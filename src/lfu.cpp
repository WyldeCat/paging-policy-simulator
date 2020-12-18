// LFU implementation

#include "page_policy.hpp"
#include <stdio.h>

using namespace std;

LFU::LFU(size_t mem_size) : PagePolicy(mem_size)
{
    cache_max_size_ = mem_size_;
    cache_size_ = 0;
    cache_ = vector<pair<long, size_t>>(cache_max_size_);
    indices_ = map<long, size_t>();
    is_evicted_ = false;
}

int LFU::add_memtrace_(const Record &record)
{
    long vpn = record.addr << 14;
    if (indices_.find(vpn) == indices_.end())
    {
        insert(cache_, indices_, vpn);
        if (is_evicted_)
            return miss_with_eviction;
        else
            return miss;
    }
    else
    {
        increment(cache_, indices_, indices_[vpn]);
        return hit;
    }
}

void LFU::swap(pair<long, size_t> &a, pair<long, size_t> &b)
{
    pair<long, size_t> temp = a;
    a = b;
    b = temp;
}

size_t LFU::get_parent_index(size_t i)
{
    return (i - 1) / 2;
}

size_t LFU::get_left_child_index(size_t i)
{
    return (2 * i) + 1;
}

size_t LFU::get_right_child_index(size_t i)
{
    return (2 * i) + 2;
}

void LFU::heapify(vector<pair<long, size_t>> &v, map<long, size_t> &m, size_t i)
{
    size_t l = get_left_child_index(i);
    size_t r = get_right_child_index(i);
    size_t minimum = 0;

    if (l < cache_size_)
    {
        if (v[i].second < v[l].second)
            minimum = i;
        else
            minimum = 1;
    }
    else
        minimum = i;

    if (r < cache_size_)
    {
        if (v[minimum].second >= v[r].second)
            minimum = r;
    }

    if (minimum != i)
    {
        m[v[minimum].first] = i;
        m[v[i].first] = minimum;
        swap(v[minimum], v[i]);
        heapify(v, m, minimum);
    }
}

void LFU::increment(vector<pair<long, size_t>> &v, map<long, size_t> &m, size_t i)
{
    ++v[i].second;
    heapify(v, m, i);
}

void LFU::insert(vector<pair<long, size_t>> &v,
                 map<long, size_t> &m, long value)
{
    if (cache_size_ == v.size())
    {
        m.erase(v[0].first);
        v[0] = v[--cache_size_];
        heapify(v, m, 0);
        is_evicted_ = true;
    }
    v[cache_size_++] = make_pair(value, 1);
    m.insert(make_pair(value, cache_size_ - 1));
    long i = cache_size_ - 1;

    while (i && v[get_parent_index(i)].second > v[i].second)
    {
        m[v[i].first] = get_parent_index(i);
        m[v[get_parent_index(i)].first] = i;
        swap(v[i], v[get_parent_index(i)]);
        i = get_parent_index(i);
    }
}