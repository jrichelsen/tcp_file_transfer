// Mitch Patin (???)
// Tim Pusateri (???)
// Jon Richelsen (jrichels)
// CSE30264
// Programming Assignment 3: TCP
// TCP Server
// Due 2015-10-13

/*
Iterations
===============
1. Copy skeleton code from lectures slides
2. Establish connection with server
    2.1. HOW TO TEST
3. Send message to server
    3.1. Read debug printout from server
4. Receive message from server
    4.1. Read debug printout on client
*/

#include <stdio.h> // printing to stdout and stderr
#include <stdlib.h> // exit status

#define DEBUG 1 // flag for whether to print debug statements

#include "tcp_shared.h" // utility functions

void print_usage(); // print correct command usage (arguments, etc.)

int main( int argc, char* argv[] )
{
    // variables and data structures
    long int port;

    analyze_argc(argc, 2, &print_usage);

    port = port_string_to_long_int(argv[1], &print_usage);
    debugprintf("port: %ld\n", port);

    return EXIT_SUCCESS;
}

void print_usage()
{
    printf("tcpserver is to be used in the following manner: I DON'T KNOW YET\n");
}
