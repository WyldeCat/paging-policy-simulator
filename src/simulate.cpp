#include "simulate.hpp"

#include "page_policy.hpp"

#include <cstdio>
#include <string>
#include <queue>
#include <unistd.h>

PagePolicy *policy;
static PIN_MUTEX *sim_lock;
std::queue<Record> record_queue;

void write_results_csv(long num_interval, long end_ts, const char *path_name) {
    const std::vector<Record> &results = policy->results();

    long interval = (end_ts + num_interval - 1) / num_interval;
    std::vector<size_t> num_access(num_interval, 0);
    std::vector<size_t> num_miss(num_interval, 0);

    for (const auto &result : results) {
        long ts = result.time_stamp;
        long idx = ts / interval;
        num_access[idx]++;
        num_miss[idx] += (result.is_hit == 0);
    }

    FILE *csv = fopen(path_name, "w");
    if (csv == nullptr) {
        perror("fopen");
        assert(false);
    }

    for (long i = 0; i < num_interval; i++) {
        fprintf(csv, "%ld, %ld, %ld\n", (i + 1) * interval, num_access[i], num_miss[i]);
    }

    int ret = fclose(csv);
    if (ret != 0) {
        perror("fclose");
        assert(false);
    }
}

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

    fprintf(stderr, "[Simulator][INFO] Simulation Finished!\n");
    fprintf(stderr, "[Simulator][INFO] Writing csv...\n");
    write_results_csv(1000, end_ts, "out.csv");
}

void add_memtrace(int is_write, long ip, long addr, long timestamp) {
    record_queue.push({is_write, ip, addr, timestamp, 0});
}
