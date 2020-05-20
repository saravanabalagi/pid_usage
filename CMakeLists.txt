project (pid-usage)
cmake_minimum_required(VERSION 2.8.9)

set(EXECUTABLE_OUTPUT_PATH ${pid-usage_SOURCE_DIR})
add_executable(pid-usage pid_usage.c src/process_args.c src/linux_get_process_info.c)

install(
    TARGETS
        pid-usage
    RUNTIME DESTINATION
        ${CMAKE_INSTALL_PREFIX}/bin
)
