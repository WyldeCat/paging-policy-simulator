#include "simulate.hpp"

#include "page_policy.hpp"

#include <cstdio>
#include <string>
#include <queue>
#include <unistd.h>

PagePolicy *policy;
static PIN_MUTEX *sim_lock;
std::queue<Record> record_queue;

void simulate_loop(void *arg) {
    simulate_args *args = (simulate_args *)arg;

    size_t mem = std::atoi(args->mem) * 1024;
    std::string policy_name(args->policy);
    sim_lock = args->lock;
    long end_ts;

    if (policy_name == "LRU") {
        policy = new LRU(mem);
    } else {
        fprintf(stderr, "[ERROR] Unknown Policy %s %zd\n",
            policy_name.c_str(), mem);
    }

    while (true) {
        Record r;
        bool is_empty = false;

        while (!PIN_MutexTryLock(sim_lock));
        if (!(is_empty = record_queue.empty())) {
            r = record_queue.front();
            record_queue.pop();
        }
        PIN_MutexUnlock(sim_lock);

        if (!is_empty && r.is_write != 2) policy->add_memtrace(r);
        else if (!is_empty && r.is_write == 2) {
            end_ts = r.time_stamp;
            break;
        }
        sleep(0);
    }

    constexpr long num_interval = 500;
    long interval = (end_ts + num_interval - 1) / num_interval;

    const std::vector<Record> &results = policy->results();
    // TODO fill buffers for graph
}

void add_memtrace(bool is_write, long ip, long addr, long timestamp) {
    record_queue.push({is_write, ip, addr, timestamp, 0});
}
