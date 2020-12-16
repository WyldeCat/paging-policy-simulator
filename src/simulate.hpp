#pragma once

#include <stddef.h>

#include "pin.H"

struct Record {
    int is_write;
    long ip;
    long addr;
    long time_stamp;
    bool is_hit;
};

struct simulate_args {
    char *mem;
    char *policy;
    PIN_MUTEX *lock;
};

void simulate_loop(void *arg);
void add_memtrace(bool is_write, long ip, long addr, long timestamp);
