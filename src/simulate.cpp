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

        if (!is_empty) policy->add_memtrace(r);
        sleep(0);
    }
}

void add_memtrace(bool is_write, long ip, long addr, long timestamp) {
    record_queue.push({is_write, ip, addr, timestamp, 0});
}
