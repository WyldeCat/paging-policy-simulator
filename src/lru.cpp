#include "page_policy.hpp"

#include <stdio.h>

LRU::LRU(size_t mem_size) : PagePolicy(mem_size), count_(0) { }

bool LRU::add_memtrace_(const Record &record) {
    count_++;
    long vpn = record.addr << 14;

    if (vpn_to_index_.find(vpn) == vpn_to_index_.end()) {
        if (vpn_to_index_.size() == max_num_page_) {
            long target = index_to_vpn_.begin()->second;
            index_to_vpn_.erase(index_to_vpn_.begin());
            vpn_to_index_.erase(target);
        }
        vpn_to_index_[vpn] = count_;
        index_to_vpn_[count_] = vpn;

        return false;
    } else {
        index_to_vpn_.erase(vpn_to_index_[vpn]);
        vpn_to_index_[vpn] = count_;
        index_to_vpn_[count_] = vpn;

        return true;
    }
}