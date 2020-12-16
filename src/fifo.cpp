#include "page_policy.hpp"

#include <stdio.h>

FIFO::FIFO(size_t mem_size) : PagePolicy(mem_size), count_(0) { }

bool FIFO::add_memtrace_(const Record &record) {
    count_++;
    long vpn = record.addr << 14;

    if (vpn_to_index_.find(vpn) == vpn_to_index_.end()) {
    	// page is not in the list
	// if cache is full
        if (vpn_to_index_.size() == max_num_page_) {
	    // erase first page
            long target = index_to_vpn_.begin()->second;
            index_to_vpn_.erase(index_to_vpn_.begin());
            vpn_to_index_.erase(target);
        }
	// insert new page
        vpn_to_index_[vpn] = count_;
        index_to_vpn_[count_] = vpn;

        return false;
    } else {
        return true;
    }
}
