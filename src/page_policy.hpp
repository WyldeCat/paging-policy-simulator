#pragma once

#include <vector>
#include <string>
#include <utility>
#include <list>
#include <set>

#include "simulate.hpp"

class PagePolicy {
public:
    PagePolicy(size_t mem_size) : mem_size_(mem_size) {
        max_num_page_ = mem_size_ / page_size;
        total_access_ = 0;
        total_hit_ = 0;
        total_eviction_ = 0;
    }

    void add_memtrace(Record &record) {
        total_access_++;

        int ret = add_memtrace_(record);
        record.is_hit = (ret == 1);
        total_hit_ += (ret == 1);
        total_eviction_ += (ret == 2);
        results_.emplace_back(std::move(record));
        evictions_.push_back((ret == 2));
    }

    const std::vector<Record> &results() { return results_; }
    const std::vector<bool> &evictions() { return evictions_; }
    virtual const char *name() = 0;

    long total_access() { return total_access_; }
    long total_hit() { return total_hit_; }
    long total_miss() { return total_access_ - total_hit_; }
    long total_eviction() { return total_eviction_; }

protected:
    static constexpr size_t page_size = 4;
    size_t mem_size_;
    size_t max_num_page_;
    std::vector<Record> results_;
    std::vector<bool> evictions_;

    long total_access_;
    long total_hit_;
    long total_eviction_;

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

class LFU : public PagePolicy {
public:
    LFU(size_t mem_size);
    virtual const char *name() override { return "LFU"; }

private:
    virtual int add_memtrace_(const Record &record) override;

    std::map<long, long> vpns_and_their_counts_;
    std::map<long, std::set<long>> counts_;

};

class CLOCKPRO : public PagePolicy
{

public:
    CLOCKPRO(size_t mem_size);
    virtual const char *name() override { return "CLOCKPRO"; }

private:
    virtual int add_memtrace_(const Record &record) override;

     char type_cold = 0;
     char type_hot = 1;
     char type_test = 2;
     char type_none = 4;

     char reference_bit_false = 0;
     char reference_bit_true = 1;
     char page_discarded = 2;

     int miss = 0;
     int hit = 1;
     int miss_with_eviction = 2;

    // sizes
    size_t mem_hot_;  // m_h == number of hot pages
    size_t mem_cold_; // m_c == number of cold pages resident
    // m = m_h+m_c

    size_t max_size_; // m
    size_t max_cold_; // m

    float hot_size_ratio_;

    std::vector<std::pair<long, char>> meta_data_;

    // char: 0 for bit false, 1 for bit true, 2 for discard
    std::map<long, char> data_cache_;

    // hand positions
    size_t hand_hot_;
    size_t hand_cold_;
    size_t hand_test_;

    //internal counters;
    size_t hot_count_;
    size_t cold_count_;
    size_t test_count_;

    size_t hot_size_;
    size_t cold_size_;


    // rest_values

    void hot_action();
    void cold_action();
    void test_action();
    void evict_pages_();


    void delete_meta_data_(long vpn, char page_type);
    void add_meta_data_(long vpn, char page_type);

    bool is_evicted;
};
