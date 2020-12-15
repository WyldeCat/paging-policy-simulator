#pragma once

#include <stddef.h>

struct simulate_args {
    char *mem;
    char *policy;
};

void simulate_loop(void *arg);
void add_memtrace(bool is_write, long ip, long addr, long timestamp);
