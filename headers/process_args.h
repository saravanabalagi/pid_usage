//
// Created by saravanabalagi on 27/04/2020.
//

#ifndef CPU_USAGE_PROCESS_ARGS_H
#define CPU_USAGE_PROCESS_ARGS_H

#include <stdbool.h>
#include <sys/types.h>
#include <inttypes.h>
#include <stdlib.h>
#include <argp.h>

struct arguments {
    pid_t pid;
    double interval;
    unsigned int repeat;
};

static error_t parse_option( int key, char *arg, struct argp_state *state );
void get_args(int argc, char** argv, struct arguments* arguments);

#endif //CPU_USAGE_PROCESS_ARGS_H
