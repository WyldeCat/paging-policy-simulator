#pragma once

#include <stddef.h>

void simulate_loop(void *arg);
void add_memtrace(bool is_write, long ip, long addr, long timestamp);
