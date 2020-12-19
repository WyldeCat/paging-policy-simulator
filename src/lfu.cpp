#include "page_policy.hpp"

#include <stdio.h>
#include <map>
#include <unordered_map>

#define PageNumber long
#define Count size_t
#define PageNumberIterator multimap<Count, PageNumber>::iterator

using namespace std;

LFU::LFU(size_t mem_size) : PagePolicy(mem_size)
{
}

int LFU::add_memtrace_(const Record &record)
{
    count_++;
    PageNumber vpn = record.addr << 14;
    auto page_iterator = page_number_unorderedmap.find(vpn);

    // the page is in the map
    if (page_iterator != page_number_unorderedmap.end())
    {
        access_page(vpn);
        return hit;
    }
    // the page is not in the map
    else
    {
        // the cache is full: eviction should happen
        if (page_number_unorderedmap.size() + 1 > mem_size_)
        {
            auto evicted_page = get_evict_target();
            remove_page(evicted_page);
            insert_page(vpn);
            return miss_with_eviction;
        }
        // the cache is not full : just add the page
        else
        {
            insert_page(vpn);
            return miss;
        }
    }
}

void LFU::insert_page(const PageNumber &page_number)
{
    page_number_unorderedmap[page_number] = counts_multimap.insert(pair<Count, PageNumber>(1, page_number));
}

void LFU::remove_page(const PageNumber &page_number)
{
    counts_multimap.erase(page_number_unorderedmap[page_number]);
    page_number_unorderedmap.erase(page_number);
}

void LFU::access_page(const PageNumber &page_number)
{
    auto page_for_update = page_number_unorderedmap[page_number];
    auto page_updated = make_pair(page_for_update->first + 1, page_for_update->second);
    counts_multimap.erase(page_for_update);
    page_number_unorderedmap[page_number] = counts_multimap.insert(move(page_updated));
}

PageNumber LFU::get_evict_target()
{
    return counts_multimap.begin()->second;
}