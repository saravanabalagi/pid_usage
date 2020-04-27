#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>

#ifdef CLOCK_MONOTONIC_RAW
#define CPU_USAGE_CLOCK CLOCK_MONOTONIC_RAW
#else
#define CPU_USAGE_CLOCK CLOCK_MONOTONIC
#endif

#define pid_path_size 1024
static char pid_path[pid_path_size];

struct process_cpu_usage {
    double user_usage;   // Seconds
    double kernel_usage; // Seconds
    size_t virtual_memory;    // Bytes
    size_t resident_memory;   // Bytes
    double total_usage;
    struct timespec measured_at;
};

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

double difftime_time(struct timespec t0, struct timespec t1) {
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

int main(int argc, char **argv) {

    if(argc == 0+1) {
        printf("Usage: cpu_usage <pid:int> [<interval:float|1.5> [<times:int|UINT16_MAX>]]");
        return 1;
    } else if(argc > 3+1) {
        printf("More than three argument provided");
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    double interval = 1.5;
    unsigned int times = UINT16_MAX;
    if(argc == 3) interval = atof(argv[2]);
    if(argc == 4) times = atoi(argv[3]);

    struct process_cpu_usage cpu_usage;
    struct timespec last_measured_at;
    double last_measured_total_usage;
    double cpu_percent;

    // First Call
    get_process_info(pid, &cpu_usage);
    last_measured_total_usage = cpu_usage.total_usage;
    last_measured_at = cpu_usage.measured_at;

    // Header CSV
    printf("Cpu Percent, Virtual Memory, Resident Memory\n");

    int i = 0;
    while(i < times) {
        usleep(interval * 1000 * 1000);
        get_process_info(pid, &cpu_usage);
        cpu_percent = 100. * (cpu_usage.total_usage - last_measured_total_usage) / difftime_time(last_measured_at, cpu_usage.measured_at);
        last_measured_total_usage = cpu_usage.total_usage;
        last_measured_at = cpu_usage.measured_at;

        // Values CSV
        printf("%.1f%%, Virtual: %zu MiB, %zu MiB\n",
                cpu_percent,
                cpu_usage.virtual_memory/(1024*1024),
                cpu_usage.resident_memory/(1024*1024));
        i++;
    }

    return 0;
}