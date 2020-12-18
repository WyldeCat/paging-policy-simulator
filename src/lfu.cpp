#include "page_policy.hpp"

#include <stdio.h>
#include <map>
#include <unordered_map>

#define PageNumber long
#define Count size_t
#define PageNumberIterator multimap<Count, PageNumber>::iterator

using namespace std

LFU::LFU(size_t mem_size) : PagePolicy(mem_size)
{
}

// structures
// counts_multimap
// page_number_unorderedmap

int LRU::add_memtrace_(const Record &record)
{
    count_++;
    long vpn = record.addr << 14
}

void LRU::insert_page(const PageNumber &page_number)
{
}

void LRU::remove_page(const PageNumber &page_number)
{
    counts_multimap.erase(page_number_unorderedmap[page_number]);
    page_number_unorderedmap.erase(page_number);
}

void LRU::access_page(const PageNumber &page_number)
{
}
