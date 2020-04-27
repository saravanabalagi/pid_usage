#include "headers/process_args.h"
#include "headers/linux_get_process_info.h"

int main(int argc, char **argv) {

    // defaults
    struct arguments arguments = { 0 };
    arguments.pid = 0;
    arguments.interval = 1.5;
    arguments.repeat = 1;
    arguments.use_header = true;

    // parse and assign arguments
    get_args(argc, argv, &arguments);
    pid_t pid = arguments.pid;
    double interval = arguments.interval;
    unsigned int repeat_times = arguments.repeat;
    bool use_header = arguments.use_header;

    // declarations
    struct process_cpu_usage cpu_usage;
    struct timespec last_measured_at;
    double last_measured_total_usage;
    double cpu_percent;

    // First Call
    get_process_info(pid, &cpu_usage);
    last_measured_total_usage = cpu_usage.total_usage;
    last_measured_at = cpu_usage.measured_at;
    usleep(0.1 * 1000 * 1000);

    // Loop for repeat_times
    int i = 0;
    while(i < repeat_times) {

        // Wait for all iterations except the first
        if(i!=0) usleep(interval * 1000 * 1000);

        // Calculate CPU Usage
        get_process_info(pid, &cpu_usage);
        cpu_percent = 100. * (cpu_usage.total_usage - last_measured_total_usage) /
                get_time_diff(last_measured_at, cpu_usage.measured_at);
        last_measured_total_usage = cpu_usage.total_usage;
        last_measured_at = cpu_usage.measured_at;

        // Header CSV
        if(i == 0)
            if(use_header)
                printf("CPU %%, Virtual Memory, Resident Memory\n");

        // Values CSV
        printf("%.1f%%, %zu MiB, %zu MiB\n",
                cpu_percent,
                cpu_usage.virtual_memory/(1024*1024),
                cpu_usage.resident_memory/(1024*1024));
        i++;
    }

    return 0;
}