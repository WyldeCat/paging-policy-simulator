#pragma once

#include <vector>
#include <string>
#include <utility>

#include "simulate.hpp"

class PagePolicy {
public:
    PagePolicy(size_t mem_size) : mem_size_(mem_size) {
        max_num_page_ = mem_size_ / page_size;
        total_access_ = 0;
        total_hit_ = 0;
        total_eviction_ = 0;
        is_eviction_happend_ = false;
    }

    void add_memtrace(Record &record) {
        total_access_++;

        int ret = add_memtrace_(record);
        record.is_hit = (ret == 1);
        total_hit_ += (ret == 1);
        total_eviction_ += (ret == 2);
        results_.push_back(record);
        evictions_.push_back(ret == 2);

        if (ret == 2 && is_eviction_happend_ == false) {
            is_eviction_happend_ = true;
            first_eviction_ = total_access_;
        }
    }

    const std::vector<Record> &results() { return results_; }
    const std::vector<int> &evictions() { return evictions_; }
    virtual const char *name() = 0;

    long total_access() { return total_access_; }
    long total_hit() { return total_hit_; }
    long total_miss() { return total_access_ - total_hit_; }
    long total_eviction() { return total_eviction_; }
    long after_eviction() {
        if (!is_eviction_happend_) return 0;
        return total_access_ - first_eviction_ + 1;
    }

protected:
    static constexpr size_t page_size = 4;
    size_t mem_size_;
    size_t max_num_page_;
    std::vector<Record> results_;
    std::vector<int> evictions_;

    long total_access_;
    long total_hit_;
    long total_eviction_;
    long first_eviction_;
    bool is_eviction_happend_ = false;

    virtual int add_memtrace_(const Record &record) = 0;
};


class List {
public:
	List();
	size_t index;
	int find(long vpn);
	void remove(long vpn);
	void remove(size_t index, long vpn);
	long pop();
	void push(long vpn);
	size_t get_size();

private:
	std::map<long, size_t> vpn_to_index;
	std::map<size_t, long> index_to_vpn;
};

class LRU : public PagePolicy {
public:
    LRU(size_t mem_size);
    virtual const char *name() override { return "LRU"; }

private:
    virtual int add_memtrace_(const Record &record) override;
    std::map<long, size_t> vpn_to_index_;
    std::map<size_t, long> index_to_vpn_;
    size_t count_;
};

class FIFO : public PagePolicy {
public:
    FIFO(size_t mem_size);
    virtual const char *name() override { return "FIFO"; }

private:
    virtual int add_memtrace_(const Record &record) override;
    std::map<long, size_t> vpn_to_index_;
    std::map<size_t, long> index_to_vpn_;
    size_t count_;
};

class ARC: public PagePolicy {
public:
    ARC(size_t mem_size);
    virtual const char *name() override { return "ARC"; }

private:
    virtual int add_memtrace_(const Record &record) override;
    void decrease_P();
    void increase_P();
    void replace(long vpn);

    size_t count_;
    List T1, T2, B1, B2;
    size_t P;
    size_t C;
};



