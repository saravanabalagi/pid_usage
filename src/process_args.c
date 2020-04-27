#include "../headers/process_args.h"

const char *argp_program_bug_address = "https://github.com/saravanabalagi/pid_usage";
const char *argp_program_version = "version 0.1";

static error_t parse_option(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    bool valid = true;

    switch ( key ) {
        case 'p': arguments->pid = atoi(arg); break;
        case 'i': arguments->interval = atof(arg); break;
        case 'r': arguments->repeat = atoi(arg); break;
        case 0x100: arguments->use_header = false; break;
        case ARGP_KEY_END:
            if(arguments->pid <= 0) valid = false;
            if(arguments->interval <= 0.0001) valid = false;
            if(arguments->repeat <= 0 || arguments->repeat >= UINT16_MAX) valid = false;
            if(!valid) argp_usage(state);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

void get_args(int argc, char **argv, struct arguments* arguments) {
    char doc[] = "A simple command line utility to find CPU and RAM Utilization "
                 "within certain intervals for a given PID. Currenly the output is CSV formatted";
    static struct argp_option options[] = {
            {"pid",   'p', "PID", 0, "(int) PID of program to monitor" },
            {"interval",   'i', "I", 0, "(float) Calculate usage every I seconds" },
            {"repeat",   'r', "R", 0, "(int) Program prints output R times" },
            {"no-header", 0x100, 0, 0, "Don't print the header row"},
            { 0 } // always terminate with a zero'd struct argp_option
    };

    struct argp argp = {options, parse_option, 0, doc};
    argp_parse(&argp, argc, argv, 0, 0, arguments);
}