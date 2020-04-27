#include "../headers/linux_get_process_info.h"

bool get_process_info(pid_t pid, struct process_cpu_usage *usage) {
    double clock_ticks_per_second = (double) sysconf(_SC_CLK_TCK);
    size_t page_size = (size_t)sysconf(_SC_PAGESIZE);
    int written = snprintf(pid_path, pid_path_size, "/proc/%" PRIdMAX "/stat",
            (intmax_t)pid);
    if (written == pid_path_size) {
        return false;
    }
    FILE *stat_file = fopen(pid_path, "r");
    if (!stat_file) {
        return false;
    }
    clock_gettime(CPU_USAGE_CLOCK, &usage->measured_at);
    unsigned long total_user_time;   // in clock_ticks
    unsigned long total_kernel_time; // in clock_ticks
    unsigned long virtual_memory;    // In bytes
    long resident_memory;            // In page number

    int return_value = fscanf(stat_file,
                              "%*d %*[^)]) %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u "
                              "%*u %lu %lu %*d %*d %*d %*d %*d %*d %*u %lu %ld",
                              &total_user_time, &total_kernel_time, &virtual_memory,
                              &resident_memory);
    fclose(stat_file);
    if (return_value != 4)
        return false;
    usage->user_usage = (double) total_user_time / clock_ticks_per_second;
    usage->kernel_usage = (double) total_kernel_time / clock_ticks_per_second;
    usage->virtual_memory = virtual_memory;
    usage->resident_memory = (size_t)resident_memory * page_size;
    usage->total_usage = usage->user_usage + usage->kernel_usage;
    return true;
}

double get_time_diff(struct timespec t0, struct timespec t1) {
    double diff = difftime(t1.tv_sec, t0.tv_sec);
    if (t1.tv_nsec < t0.tv_nsec) {
        long val = 1000000000l - t0.tv_nsec + t1.tv_nsec;
        diff += (double)val / 1e9 - 1.;
    } else {
        long val = t1.tv_nsec - t0.tv_nsec;
        diff += (double)val / 1e9;
    }
    return diff;
}