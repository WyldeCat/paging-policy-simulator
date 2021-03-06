#include "page_policy.hpp"

#include <stdio.h>

FIFO::FIFO(size_t mem_size) : PagePolicy(mem_size), count_(0) { }

int FIFO::add_memtrace_(const Record &record) {
    long vpn = record.addr << 14;
    if (vpn_to_index_.find(vpn) == vpn_to_index_.end()) {
        count_++;
        int ret = 0;
    	// page is not in the list
        if (vpn_to_index_.size() == max_num_page_) {
	    // if cache is full
	    // erase first page
            long target = index_to_vpn_.begin()->second;
            index_to_vpn_.erase(index_to_vpn_.begin());
            vpn_to_index_.erase(target);
	    ret = 2;
        }
	// insert new page
        vpn_to_index_[vpn] = count_;
        index_to_vpn_[count_] = vpn;

        return ret;
    } else {
    	// page is in the list
        return 1;
    }
}
