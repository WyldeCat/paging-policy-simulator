#include "simulate.hpp"

#include "page_policy.hpp"

#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <unistd.h>

PagePolicy *policy;

std::vector<std::vector<Record>> record_queue;
PIN_MUTEX lock;
std::set<int> buf_free;
std::set<int> buf_full;
size_t curr_idx = 0;
size_t curr_cnt = 0;
bool terminate = false;
volatile bool is_initialized = false;
size_t num_buffer;
size_t size_buffer;
volatile long offset;

long get_timestamp();

void write_results_csv(long num_interval, long end_ts, const char *path_name) {
    const std::vector<Record> &results = policy->results();
    const std::vector<bool> &evictions = policy->evictions();

    long interval = ((end_ts - offset) + num_interval - 1) / num_interval;
    std::vector<size_t> num_access(num_interval, 0);
    std::vector<size_t> num_miss(num_interval, 0);
    std::vector<size_t> num_eviction(num_interval, 0);

    for (size_t i = 0; i < results.size(); i++) {
        const Record &result = results[i];
        const bool &eviction = evictions[i];

        long ts = std::max(0L, result.time_stamp - offset);
        long idx = ts / interval;
        num_access[idx]++;
        num_miss[idx] += (result.is_hit == 0);
        num_eviction[idx] += eviction;
    }

    FILE *csv = fopen(path_name, "w");
    if (csv == nullptr) {
        perror("fopen");
        assert(false);
    }

    for (long i = 0; i < num_interval; i++) {
        fprintf(csv, "%ld, %ld, %ld, %ld\n", (i + 1) * interval,
            num_access[i], num_miss[i], num_eviction[i]);
    }

    int ret = fclose(csv);
    if (ret != 0) {
        perror("fclose");
        assert(false);
    }
}

void simulate_loop(void *arg) {
    simulate_args *sim_args;
    sim_args = (simulate_args *)arg;

    // prepare buffers and lock
    PIN_MutexInit(&lock);
    num_buffer = std::atoi(sim_args->num_buffer);
    size_buffer = std::atoi(sim_args->size_buffer) * 1024L * 1024L;
    record_queue.resize(num_buffer, std::vector<Record>(size_buffer));
    curr_idx = curr_cnt = 0;
    for (size_t i = 0; i < num_buffer; i++) buf_free.insert(i);
    terminate = false;

    is_initialized = true;

    // check timestamp after initialization
    offset = get_timestamp();

    // create policy instance
    size_t mem = std::atoi(sim_args->mem) * 1024;
    std::string policy_name(sim_args->policy);
    long end_ts = 0;

    if (policy_name == "LRU") {
        policy = new LRU(mem);
    } else if( policy_name == "ARC" ) {
        policy = new ARC(mem);
    } else if( policy_name == "FIFO" ) {
        policy = new FIFO(mem);
    } else if( policy_name == "LFU" ) {
    } else if( policy_name == "CLOCK_PRO" ) {
    } else {
        fprintf(stderr, "[ERROR] Unknown Policy %s %zd\n",
            policy_name.c_str(), mem);
    }

    // trace consume loop
    while (!terminate) {
        bool is_empty;
        int target;

        while (!PIN_MutexTryLock(&lock));
        is_empty = buf_full.empty();
        if (is_empty) {
            PIN_MutexUnlock(&lock);
            sleep(1);
            continue;
        } else {
            target = *buf_full.begin();
            buf_full.erase(target);
        }
        PIN_MutexUnlock(&lock);

        for (auto &r : record_queue[target]) {
            if (r.is_write == 2) {
                end_ts = r.time_stamp;
            }
            policy->add_memtrace(r);
        }

        while (!PIN_MutexTryLock(&lock));
        buf_free.insert(target);
        PIN_MutexUnlock(&lock);
    }

    for (size_t i = 0; i < curr_cnt; i++) {
        if (record_queue[curr_idx][i].is_write == 2) {
            end_ts = record_queue[curr_idx][i].time_stamp;
        }
        policy->add_memtrace(record_queue[curr_idx][i]);
    }

    for (int f : buf_full) {
        for (auto &r : record_queue[f]) {
            if (r.is_write == 2) end_ts = r.time_stamp;
            policy->add_memtrace(r);
        }
    }

    fprintf(stderr, "[Simulator][INFO] Simulation Finished!\n\n");
    fprintf(stderr, "[Simulator][INFO]      Elapsed(ms) : %f\n", (end_ts - offset) / 1000.0);
    fprintf(stderr, "[Simulator][INFO] Total mem access : %ld\n", policy->total_access());
    fprintf(stderr, "[Simulator][INFO]              hit : %ld\n", policy->total_hit());
    fprintf(stderr, "[Simulator][INFO]             miss : %ld\n", policy->total_miss());
    fprintf(stderr, "[Simulator][INFO]         eviction : %ld\n", policy->total_eviction());
    fprintf(stderr, "[Simulator][INFO]        Hit ratio : %f\n\n",
        1.0 * policy->total_hit() / policy->total_access());

    fprintf(stderr, "[Simulator][INFO] Writing csv...\n");
    write_results_csv(100, end_ts, "out.csv");
    fprintf(stderr, "[Simulator][INFO] Finish!\n");
}

void add_memtrace(const Record &r) {
    if (!is_initialized) {
        while (!is_initialized);
    }

    record_queue[curr_idx][curr_cnt++] = r;
    if (curr_cnt == size_buffer) {
        bool is_free_empty;
        int next = 0;

        while (!PIN_MutexTryLock(&lock));
        fprintf(stderr, "- insert full!! %zd\n", curr_idx);
        buf_full.insert(curr_idx);
        PIN_MutexUnlock(&lock);

        while (true) {
            while (!PIN_MutexTryLock(&lock));
            is_free_empty = buf_free.empty();
            if (!is_free_empty) {
                next = *buf_free.begin();
                buf_free.erase(next);
                PIN_MutexUnlock(&lock);
                break;
            }
            PIN_MutexUnlock(&lock);
            sleep(1);
        }

        curr_idx = next;
        curr_cnt = 0;
    }

    if (r.is_write == 2) {
        terminate = true;
    }
}
