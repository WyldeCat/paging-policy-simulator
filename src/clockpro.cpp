#include "page_policy.hpp"

#include <stdio.h>
#include <map>
#include <unordered_map>

#define PageNumber long
#define Count size_t

#define type_cold 0
#define type_hot 1
#define type_test 2

#define in_test true
#define not_in_test false

#define ref_bit_on true
#define ref_bit_off false

// std::map<long, std::pair<bool, bool> page_bit_and_is_test_map;
// std::vector<std::pair<long, char> page_type_vector;

using namespace std;

CLOCK_PRO::CLOCK_PRO(size_t mem_size) : PagePolicy(mem_size), count_(0)
{
    hand_cold = 0;
    hand_hot = 0;
    hand_test = 0;

    count_cold = 0;
    count_hot = 0;
    count_test = 0;
    ret = miss;
}

int CLOCK_PRO::add_memtrace_(const Record &record)
{
    ret = miss;
    count_++;
    PageNumber vpn = record.addr << 14;

    auto page_iterator = page_bit_and_is_test_map.find(vpn);

    // the page is in the map
    if (page_iterator != page_bit_and_is_test_map.end())
    {
        // case 0: the page is not in test
        if (page_bit_and_is_test_map[vpn].second != in_test)
        {
            page_bit_and_is_test_map[vpn].first = ref_bit_on;
            return hit;
        }
        // the page is not in the cache
        // case 1: the page is in test
        else
        {
            if (mem_cold_size < mem_size_)
                mem_cold_size++;
            page_bit_and_is_test_map[vpn] = make_pair(ref_bit_on, not_in_test);
            delete_meta(make_pair(vpn, type_test));
            count_test--;
            add_meta(make_pair(vpn, type_hot));
            count_hot++;
            return ret;
        }
    }
    // the page is not in the map
    else
    {
        insert_page(vpn);
        return ret;
    }
}

void CLOCK_PRO::insert_page(const PageNumber &page_number)
{
    page_bit_and_is_test_map[page_number] = make_pair(ref_bit_off, not_in_test);
    add_meta(make_pair(page_number, type_cold));
    count_cold++;
}

void CLOCK_PRO::evict_pages()
{
    while (mem_size_ <= count_hot + count_cold)
        cold_action();
}

void CLOCK_PRO::add_meta(pair<long, char> meta_data)
{
    evict_pages();
    page_type_vector.insert(page_type_vector.begin() + hand_hot, meta_data);

    size_t index = hand_hot;
    size_t max_position = page_type_vector.size() - 1;

    if (hand_hot >= index)
    {
        hand_hot++;
        if (hand_hot >= max_position)
            hand_hot = 0;
    }

    if (hand_cold >= index)
    {
        hand_cold++;
        if (hand_cold >= max_position)
            hand_cold = 0;
    }

    if (hand_test >= index)
    {
        hand_test++;
        if (hand_test >= max_position)
            hand_test = 0;
    }
}

void CLOCK_PRO::delete_meta(pair<long, char> meta_data)
{
    auto page_iterator = find(page_type_vector.begin(), page_type_vector.end(), meta_data);
    size_t index = page_iterator - page_type_vector.begin();
    page_type_vector.erase(&meta_data);
    size_t max_position = page_type_vector.size() - 1;

    if (hand_hot >= index)
    {
        hand_hot--;
        if (hand_hot < 0)
            hand_hot = max_position;
    }

    if (hand_cold >= index)
    {
        hand_cold--;
        if (hand_cold < 0)
            hand_cold = max_position;
    }

    if (hand_test >= index)
    {
        hand_test--;
        if (hand_test < 0)
            hand_test = max_position;
    }
}

void CLOCK_PRO::cold_action()
{
    auto meta_data = page_type_vector[hand_cold];
    auto ref_bit_and_test_bit = page_bit_and_is_test_map[meta_data.first];
    if (meta_data.second == type_cold)
    {
        bool ref_bit = ref_bit_and_test_bit.first;
        if (ref_bit)
        {
            meta_data.second = type_hot;
            ref_bit = false;
            count_cold--;
            count_hot++;
        }
        else
        {
            //eviction happened
            ret = miss_with_eviction;
            meta_data.second = type_test;
            ref_bit_and_test_bit.second = in_test;
            count_cold--;
            count_test++;

            while (mem_size_ < count_test)
            {
                test_action();
            }
        }

        hand_cold++;
        if (hand_cold >= page_type_vector.size())
            hand_cold = 0;
        while (mem_size_ - mem_cold_size < count_hot)
        {
            hot_action();
        }
    }
}

void CLOCK_PRO::hot_action()
{
    if (hand_hot == hand_test)
        test_action();

    auto meta_data = page_type_vector[hand_hot];
    if (meta_data.second == type_hot)
    {
        bool ref_bit = page_bit_and_is_test_map[meta_data.first].first;
        if (ref_bit)
            ref_bit = false;
        else
        {
            meta_data.second = type_cold;
            count_hot--;
            count_cold++;
        }
    }
    hand_hot++;
    if (hand_hot >= page_type_vector.size())
        hand_hot = 0;
}

void CLOCK_PRO::test_action()
{
    if (hand_test == hand_cold)
        cold_action();
    auto meta_data = page_type_vector[hand_test];

    if (meta_data.second == type_test)
    {
        //remove the page
        page_bit_and_is_test_map.erase(meta_data.first);
        delete_meta(meta_data);

        count_test--;
        if (mem_cold_size > 1)
            mem_cold_size--;
    }

    hand_test++;
    if (hand_test >= page_type_vector.size())
        hand_test = 0;
}