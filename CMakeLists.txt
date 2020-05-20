cmake_minimum_required(VERSION 2.8.9)
project (pid-usage)
#add_executable(pid-usage.c src/process_args.c src/linux_get_process_info.c)
add_executable(pid-usage pid_usage.c src/process_args.c src/linux_get_process_info.c)
