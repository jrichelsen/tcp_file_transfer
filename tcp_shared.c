#include <stdio.h> // printing to stdout and stderr
#include <stdarg.h> // handle variable arguments for debug print
#include <stdlib.h> // exit status
#include <limits.h> // type limits for checking strtol
#include <errno.h> // error status for checking strtol

#define DEBUG 1 // flag for whether to print debug statements

#include "tcp_shared.h"

void debugprintf(const char * const format, ...)
{
    if (!DEBUG) {
        return;
    }

    va_list args;
    va_start(args, format);
    vfprintf (stderr, format, args);
    return;
}

long int port_string_to_long_int(const char * const string_value, void (* print_usage_ptr)())
{
    char * end_ptr;
    errno = 0;
    long int long_int_value = strtol(string_value, &end_ptr, 10);

    if ((errno == ERANGE && (long_int_value == LONG_MAX || long_int_value == LONG_MIN)) || (errno != 0 && long_int_value == 0)) {
        fprintf(stderr, "Error converting port number \"%s\" to integer. Exiting now.\n", string_value);
        print_usage_ptr();
        exit(EXIT_FAILURE);
    }
    if (end_ptr == string_value || *end_ptr != '\0') {
        fprintf(stderr, "Port numbered entered \"%s\" cannot be converted to integer. Exiting now.\n", string_value);
        print_usage_ptr();
        exit(EXIT_FAILURE);
    }

    return long_int_value;
}

void analyze_argc(int argc, int argc_expected, void (* print_usage_ptr)())
{
    if (argc == 1) {
        fprintf(stderr, "No command line arguments specified. Exiting now.\n");
        print_usage_ptr();
        exit(EXIT_FAILURE);
    } else if (argc != argc_expected) {
        fprintf(stderr, "Incorrect number of command line arguments. Exiting now.\n");
        print_usage_ptr();
        exit(EXIT_FAILURE);
    }
}
