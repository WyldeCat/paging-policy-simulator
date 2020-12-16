#pragma once

#include <vector>
#include <string>
#include <utility>

#include "simulate.hpp"

class PagePolicy {
public:
    PagePolicy(size_t mem_size) : mem_size_(mem_size) {
        max_num_page_ = mem_size_ / page_size;
    }

    void add_memtrace(Record &record) {
        record.is_hit = add_memtrace_(record);
        results_.emplace_back(std::move(record));
    }

    virtual const char *name() = 0;

protected:
    static constexpr size_t page_size = 4;
    size_t mem_size_;
    size_t max_num_page_;
    std::vector<Record> results_;

    virtual bool add_memtrace_(const Record &record) = 0;
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
    virtual bool add_memtrace_(const Record &record) override;
    std::map<long, size_t> vpn_to_index_;
    std::map<size_t, long> index_to_vpn_;
    size_t count_;
};

class FIFO : public PagePolicy {
public:
    FIFO(size_t mem_size);
    virtual const char *name() override { return "FIFO"; }

private:
    virtual bool add_memtrace_(const Record &record) override;
    std::map<long, size_t> vpn_to_index_;
    std::map<size_t, long> index_to_vpn_;
    size_t count_;
};

class ARC: public PagePolicy {
public:
    ARC(size_t mem_size);
    virtual const char *name() override { return "ARC"; }

private:
    virtual bool add_memtrace_(const Record &record) override;
    void decrease_P();
    void increase_P();
    void replace(long vpn);

    size_t count_;
    List T1, T2, B1, B2;
    size_t P;
    size_t C;
};



