#include <stdio.h> // printing to stdout and stderr
#include <stdlib.h> // exit status
#include <stdarg.h> // handle variable arguments for debug print
#include <stdint.h> // uint16_t (port type)
#include <errno.h> // error status for checking strtol
#include <limits.h> // type limits for checking strtol

#include "tcp_shared.h"

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

void debugprintf(const char * const format, ...)
{
    if (!DEBUG) {
        return;
    }

    va_list args;
    va_start(args, format);
    fprintf(stderr, "DEBUG: ");
    vfprintf (stderr, format, args);
    fprintf(stderr, "\n");
}

uint16_t port_string_to_uint16_t(const char * const string_value, void (* print_usage_ptr)())
{
    char * end_ptr;
    errno = 0;
    uint16_t long_int_value = strtol(string_value, &end_ptr, 10);

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
