#include "page_policy.hpp"

#include <stdio.h>

// 0을 return하면 miss, 1을 return하면 hit, 2를 return하면 miss+eviction

CLOCKPRO::CLOCKPRO(size_t mem_size, size_t hot_size_ratio) : PagePolicy(mem_size), hot_size_ratio_(hot_size_ratio)
{
    hot_size_ = mem_size_ * hot_size_ratio_;
    cold_size_ = mem_size_ - hot_size_;

    hand_hot_ = 0;
    hand_cold_ = 0;
    hand_test_ = 0;

    hot_count_ = 0;
    cold_count_ = 0;
    test_count_ = 0;

    is_evicted = false;
}

int CLOCKPRO::add_memtrace_(const Record &record)
{
    long vpn = record.addr << 14;

    // check if the page is in the list
    if (data_cache_.find(vpn) != data_cache_.end())
    {
        if (data_cache_[vpn] != data_discarded)
        {
            if (cold_count_ < max_size_)
                cold_count_++;

            data_cache_[vpn] = reference_bit_false;

            // test
            delete_meta_data_(new std::pair<long, char>(vpn, type_test));
            test_count_--;

            add_meta_data_(new std::pair<long, char>(vpn, type_hot));
            hot_count_++;

            return hit;
        }
        else
        {
            data_cache_[key] = reference_bit_true;
            return miss;
        }
    }

    else
    {
        data_cache_[vpn] = reference_bit_false;
        add_meta_data_(new std::pair<long, char>(vpn, type_cold));
        cold_count_++;

        if(is_evicted) return miss_with_eviction;
        else return miss;
    }
}

void CLOCKPRO::delete_meta_data_(pair<long, char> meta_datum)
{
    size_t meta_index = meta_data_.find(meta_datum);
    meta_data_.erase(meta_datum);
    int max_position = meta_data_.size() - 1;

    // if the hot hand
    if (hand_hot_ >= meta_index)
    {
        hand_hot_--;
        if (hand_hot_ < 0)
            hand_hot_ = max_position;
    }

    if (hand_cold_ >= meta_index)
    {
        hand_cold_--;
        if (hand_cold_ < 0)
            hand_cold_ = max_position;
    }

    if (hand_test_ >= meta_index)
    {
        hand_test_--;
        if (hand_test_ < 0)
            hand_test_ = max_position;
    }
}

void CLOCKPRO::add_meta_data_(pair<long, char> meta_datum)
{
    evict_pages_();
    meta_data_.insert(hand_hot_, meta_datum);
    int max_position = meta_data_.size();

    if (hand_cold_ > hand_hot_)
    {
        hand_cold_++;
        if (hand_cold_ > max_position)
            hand_cold_ = 0;
    }

    if (hand_test_ > hand_hot)
    {
        hand_test_++;
        if (hand_test_ > max_position)
            hand_test_ = 0;
    }

    hand_hot_++;
    if (hand_hot_ > max_position)
        hand_hot_ = 0;
}

void CLOCKPRO::evict_pages_()
{
    while (max_size_ <= cold_count_ + hot_count_){
        cold_action();
        is_evicted = true;
    }
}

void CLOCKPRO::cold_action()
{
    std::pair<long, char> meta_datum = meta_data[hand_cold_];
    long current_page_num = meta_datum.first;
    // if cold
    if (meta_datum.second == type_cold)
    {
        if (data_cache_[current_page_num] == reference_bit_true)
        {
            meta_datum.second = type_hot;
            data_cache_[current_page_num] = reference_bit_false;

            hot_count_++;
            cold_count_--;
        }

        // ref bit is 0
        else
        {
            // change the type to test
            meta_datum.second = type_test;
            data_cache_[current_page_num] = page_discarded;
            cold_count_--;
            test_count_++;

            while (max_size_ < test_count_)
                test_action();
        }

        hand_cold_++;
        if (hand_cold_ >= meta_data_.size())
            hand_cold_ = 0;

        while (max_size_ - max_cold_ < hot_count_)
            hot_action();
    }
}

void CLOCKPRO::hot_action(long current_page_num)
{
    if (hand_hot_ == hand_test_)
        test_action();

    std::pair<long, char> meta_datum = meta_data_[hand_hot_];
    long current_page_num = meta_datum.first;

    // if hot
    if (meta_datum.second = 1)
    {
        // set the reference bit
        if (data_cache_[current_page_num] == reference_bit_true)
            data_cache_[current_page_num] = reference_bit_false;

        else
        {
            meta_datum.second = type_cold;
            data_cache_[current_page_num] = reference_bit_false;
            hot_count_--;
            cold_count_++;
        }
    }
    hand_hot_++;
    if (hand_hot_ >= meta_data_.size())
    {
        hand_hot_ = 0;
    }
}

void CLOCKPRO::test_action()
{
    if (hand_test_ == hand_cold_)
        cold_action();
        
    std::pair<long, size_t> meta_datum = meta_data_[hand_test_];
    //if test
    if (meta_datum.second == type_test)
    {
        data_cache_.erase(meta_datum.first);
        delete_meta_data_(meta_datum);
        test_count_--;
        if (max_cold_ > 1)
            max_cold_--;
    }
    hand_test_++;
    if (hand_test_ >= meta_data_.size())
    {
        hand_test_ = 0;
    }
}