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
    }

    void add_memtrace(Record &record) {
        total_access_++;

        record.is_hit = add_memtrace_(record);

        total_hit_ += record.is_hit;
        results_.emplace_back(std::move(record));
    }

    const std::vector<Record> &results() { return results_; }
    virtual const char *name() = 0;

    long total_access() { return total_access_; }
    long total_hit() { return total_hit_; }
    long total_miss() { return total_access_ - total_hit_; }

protected:
    static constexpr size_t page_size = 4;
    size_t mem_size_;
    size_t max_num_page_;
    std::vector<Record> results_;

    long total_access_;
    long total_hit_;

    virtual bool add_memtrace_(const Record &record) = 0;
};

class LRU : public PagePolicy {
public:
    LRU(size_t mem_size);
    virtual const char *name() override { return "LRU"; }

private:
    virtual bool add_memtrace_(const Record &record) override;
    std::map<long, size_t> vpn_to_index_;
    std::map<size_t, long> index_to_vpn_;
    size_t count_;
};
