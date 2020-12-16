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
    char *num_buffer;
    char *size_buffer;
};

void simulate_loop(void *arg);
void add_memtrace(const Record &r);
