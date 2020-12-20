#include "simulate.hpp"

#include "page_policy.hpp"

#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <queue>
#include <unistd.h>

PagePolicy *policy;

std::vector<std::vector<Record>> record_queue;
PIN_MUTEX lock;
std::set<int> buf_free;
std::queue<int> buf_full;
size_t curr_idx = 0;
size_t curr_cnt = 0;
bool terminate = false;
volatile bool is_initialized = false;
size_t num_buffer;
size_t size_buffer;

static struct timeval start;
static long get_timestamp() {
    struct timeval now;
    gettimeofday(&now, NULL);

    return (now.tv_sec - start.tv_sec) * 1000000L +
        (now.tv_usec - start.tv_usec);
}

void write_results_csv(long num_interval, long end_ts, const char *path_name)
{
    const std::vector<Record> &results = policy->results();
    const std::vector<int> &evictions = policy->evictions();

    long interval = (end_ts + num_interval - 1) / num_interval;
    std::vector<size_t> num_access(num_interval, 0);
    std::vector<size_t> num_miss(num_interval, 0);
    std::vector<size_t> num_eviction(num_interval, 0);

    for (size_t i = 0; i < results.size(); i++)
    {
        const Record &result = results[i];
        const int &eviction = evictions[i];

        long ts = result.time_stamp;
        long idx = ts / interval;
        num_access[idx]++;
        num_miss[idx] += (result.is_hit == 0);
        num_eviction[idx] += (eviction == 1);
    }

    FILE *csv = fopen(path_name, "w");
    if (csv == nullptr)
    {
        perror("fopen");
        assert(false);
    }

    for (long i = 0; i < num_interval; i++)
    {
        fprintf(csv, "%ld, %ld, %ld, %ld\n", (i + 1) * interval,
                num_access[i], num_miss[i], num_eviction[i]);
    }

    int ret = fclose(csv);
    if (ret != 0)
    {
        perror("fclose");
        assert(false);
    }
}

void simulate_loop(void *arg) {
    gettimeofday(&start, NULL);

    simulate_args *sim_args;
    sim_args = (simulate_args *)arg;

    // prepare buffers and lock
    PIN_MutexInit(&lock);
    num_buffer = std::atoi(sim_args->num_buffer);
    size_buffer = std::atoi(sim_args->size_buffer) * 1024L * 1024L;
    record_queue.resize(num_buffer, std::vector<Record>(size_buffer));
    curr_idx = curr_cnt = 0;
    for (size_t i = 1; i < num_buffer; i++) buf_free.insert(i);
    terminate = false;

    is_initialized = true;

    // create policy instance
    size_t mem = std::atoi(sim_args->mem) * 1024;
    std::string policy_name(sim_args->policy);
    long end_ts = 0;

    if (policy_name == "LRU")
    {
        policy = new LRU(mem);
    }
    else if (policy_name == "ARC")
    {
        policy = new ARC(mem);
    }
    else if (policy_name == "FIFO")
    {
        policy = new FIFO(mem);
    }
    else if (policy_name == "LFU")
    {
        policy = new LFU(mem);
    }
    else if (policy_name == "CLOCK_PRO")
    {
        policy = new CLOCK_PRO(mem);
    }
    else
    {
        fprintf(stderr, "[ERROR] Unknown Policy %s %zd\n",
                policy_name.c_str(), mem);
    }

    // trace consume loop
    while (!terminate) {
        int target;

        while (!PIN_MutexTryLock(&lock));
        if (buf_full.empty()) {
            PIN_MutexUnlock(&lock);
            sleep(0);
            continue;
        } else {
            target = buf_full.front();
            buf_full.pop();
        }
        PIN_MutexUnlock(&lock);

        for (size_t i = 0; i < record_queue[target].size(); i++) {
            auto &r = record_queue[target][i];
            if (r.is_write == 2) {
                end_ts = r.time_stamp;
            }
            policy->add_memtrace(r);
        }

        while (!PIN_MutexTryLock(&lock))
            ;
        buf_free.insert(target);
        PIN_MutexUnlock(&lock);
    }

    while (!buf_full.empty()) {
        int f = buf_full.front();
        buf_full.pop();
        for (auto &r : record_queue[f]) {
            if (r.is_write == 2) end_ts = r.time_stamp;
            policy->add_memtrace(r);
        }
    }

    for (size_t i = 0; i < curr_cnt; i++) {
        if (record_queue[curr_idx][i].is_write == 2) {
            end_ts = record_queue[curr_idx][i].time_stamp;
        }
        policy->add_memtrace(record_queue[curr_idx][i]);
    }

    long total = get_timestamp();
    fprintf(stderr, "[Simulator][INFO]  Simulation Finished!\n\n");
    fprintf(stderr, "[Simulator][INFO] Physical Mem(KiB) : %zd\n", mem);
    fprintf(stderr, "[Simulator][INFO]       Elapsed(ms) : %f\n", end_ts / 1000.0);
    fprintf(stderr, "[Simulator][INFO] Total elapsed(ms) : %f\n", total / 1000.0);
    fprintf(stderr, "[Simulator][INFO]  Total mem access : %ld\n", policy->total_access());
    fprintf(stderr, "[Simulator][INFO]access after evict : %ld\n", policy->after_eviction());
    fprintf(stderr, "[Simulator][INFO]               hit : %ld\n", policy->total_hit());
    fprintf(stderr, "[Simulator][INFO]              miss : %ld\n", policy->total_miss());
    fprintf(stderr, "[Simulator][INFO]          eviction : %ld\n", policy->total_eviction());
    fprintf(stderr, "[Simulator][INFO]         Hit ratio : %f\n\n",
        1.0 * policy->total_hit() / policy->total_access());
    if (policy->after_eviction() != 0) {
        fprintf(stderr, "[Simulator][INFO]        Hit ratio' : %f\n\n",
            1.0 - 1.0 * policy->total_eviction() / policy->after_eviction());
    }

    fprintf(stderr, "[Simulator][INFO] Writing csv...\n");
    write_results_csv(std::atoi(sim_args->num_interval), end_ts, sim_args->csv_out);
    fprintf(stderr, "[Simulator][INFO] Finish!\n");
}

void add_memtrace(const Record &r)
{
    if (!is_initialized)
    {
        while (!is_initialized)
            ;
    }

    record_queue[curr_idx][curr_cnt++] = r;

    if (curr_cnt == size_buffer) {
        bool is_free_empty;
        int next = 0;

        while (!PIN_MutexTryLock(&lock));
        buf_full.push(curr_idx);
        PIN_MutexUnlock(&lock);

        while (true)
        {
            while (!PIN_MutexTryLock(&lock))
                ;
            is_free_empty = buf_free.empty();
            if (!is_free_empty)
            {
                next = *buf_free.begin();
                buf_free.erase(next);
                PIN_MutexUnlock(&lock);
                break;
            }
            PIN_MutexUnlock(&lock);
            sleep(0);
        }

        curr_idx = next;
        curr_cnt = 0;
    }

    if (r.is_write == 2)
    {
        terminate = true;
    }
}
