#include "simulate.hpp"

#include <stdio.h>

void simulate_loop(void *arg) {
    simulate_args *args = (simulate_args *)arg;

    printf("Hello, cpp! %s %s\n", args->mem, args->policy);
}

void add_memtrace(bool is_write, long ip, long addr, long timestamp) {

}
