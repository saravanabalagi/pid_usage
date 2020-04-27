cmake_minimum_required(VERSION 2.8.9)
project (cpu_usage)
add_executable(cpu_usage.c src/process_args.c src/linux_get_process_info.c)