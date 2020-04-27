//
// Created by saravanabalagi on 27/04/2020.
//

#ifndef CPU_USAGE_LINUX_GET_PROCESS_INFO_H
#define CPU_USAGE_LINUX_GET_PROCESS_INFO_H

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>
#include <inttypes.h>
#include <unistd.h>

#ifdef CLOCK_MONOTONIC_RAW
#define CPU_USAGE_CLOCK CLOCK_MONOTONIC_RAW
#else
#define CPU_USAGE_CLOCK CLOCK_MONOTONIC
#endif

#define pid_path_size 1024
static char pid_path[pid_path_size];

struct process_cpu_usage {
    // obtained from /proc/pid/stat
    double user_usage;   // Seconds
    double kernel_usage; // Seconds
    size_t virtual_memory;    // Bytes
    size_t resident_memory;   // Bytes

    // calculated
    double total_usage;  // sum of usages
    struct timespec measured_at;
};

bool get_process_info(pid_t pid, struct process_cpu_usage *usage);
double get_time_diff(struct timespec t0, struct timespec t1);

#endif //CPU_USAGE_LINUX_GET_PROCESS_INFO_H
