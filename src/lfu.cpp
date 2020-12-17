// LFU implementation

#include "page_policy.hpp"

#include <stdio.h>

LFU::LFU(size_t mem_size) : PagePolicy(mem_size) {}

int LFU::add_memtrace_(const Record &record)
{
    long vpn = record.addr << 14;

    // if the page isn't present
    if (vpns_and_their_counts_.find(vpn) == vpns_and_their_counts_.end())
    {

        // case 0 : the cache is full
        if (vpns_and_their_counts.size() == max_num_page_)
        {
            // find the target to evict
            long smallest_count = counts_.begin()->first;
            set<long> set_with_the_count = counts_.begin()->second;
            long target = set_with_the_count.begin();

            // if there's no left vpn for the count
            if (set_with_the_count.size() == 1)
            {
                counts_.erase(smallest_count);
            }

            // if there are more than 2 vpns
            else
            {
                set_with_the_count.erase(target);
                vpns_and_their_counts_.erase(target);
            }

            // now the eviction is finished
            // let's add the new page

            // if there is no room for "1"
            if (counts_.find(1) == counts_.end())
            {
                counts_.insert(std::pair<long, long>(1, std::set<long>{vpn}));
            }

            // if there is a room
            else
            {
                set<long> set_for_count_1 = counts_[1];
                set_for_count_1.insert(vpn);
            }
            vpns_and_their_counts_.insert(std::pair<long, long>(vpn, 1));
        }
        return 0;
    }

    // if the page is present
    else
    {
        long count_for_the_vpn = vpns_and_their_counts_[vpn];
        set<long> set_for_the_count = counts_[count_for_the_vpn];
        set_for_the_count.erase(count_for_the_vpn);

        count_for_the_vpn++;

        // if there is no room for "count+1"
        if (counts_.find(count_for_the_vpn) == counts_.end())
        {
            counts_.insert(std::pair<long, long>(count_for_the_vpn, std::set<long>{vpn}));
        }

        // if there is a room
        else
        {
            set<long> set_for_count_incr_ = counts_[count_for_the_vpn];
            set_for_count_incr_.insert(vpn);
        }

        return 1;
    }
}
