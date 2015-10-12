// Mitch Patin (???)
// Tim Pusateri (???)
// Jon Richelsen (jrichels)
// CSE30264
// Programming Assignment 3: TCP
// TCP Client
// Due 2015-10-13

/*
Iterations
===============
1. Copy skeleton code from lectures slides
2. Establish connection with server
3. Send message to server
    3.1. Read debug printout from server
4. Receive message from server
    4.1. Read debug printout on client
5. File searching on server
    5.1. Accurate output on whether file exists
6. File sending (no hashing)
    6.1. Verify contents of file
7. Hashing
*/

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>

#include "tcp_shared.h" // utility functions

int DEBUG = 1; // flag for whether to print debug statements
#define BUF_LEN 1000000 // length (in characters) of buffer used to receive and send messages

void print_usage(); // print correct command usage (arguments, etc.)

int main( int argc, char * argv[] )
{
    // variables and data structures
    const char * server_name; // server name (from command line)
    const char * port_str; // socket (from command line)
    const char * filename; // filename of file to send (from command line)
    size_t filename_len; // length of filename string
    struct addrinfo hints; // hints for getaddrinfo()
    struct addrinfo * server_info_ptr; // linked list returned by getaddrinfo()
    struct addrinfo * this_addr_ptr; // pointer to current item on server_info_ptr linked list
    int socket_fd; // socket for communicating with server
    char buf[BUF_LEN]; // buffer used to receive and send messages
    size_t buf_size = sizeof(buf); // size of above buffer

    // get information from command line
    analyze_argc(argc, 4, &print_usage);
    server_name = argv[1];
    debugprintf("server name argument: %s", server_name);
    port_str = argv[2];
    debugprintf("port argument: %s", port_str);
    filename = argv[3];
    debugprintf("filename argument: %s", filename);
    filename_len = strlen(filename);
    debugprintf("filename length: %z", filename_len);

    // get address of server
    memset(&hints, 0, sizeof(hints)); // clear hints
    hints.ai_family = AF_UNSPEC; // return both IPv4 and IPv6 addresses
    hints.ai_socktype = SOCK_STREAM; // specify TCP

    int gai_rv;
    if ((gai_rv = getaddrinfo(server_name, port_str, &hints, &server_info_ptr)) != 0) {
        fprintf(stderr, "error getting address information for server: %s\n", gai_strerror(gai_rv));
        exit(EXIT_FAILURE);
    }

    // connect to first address where binding is possible
    for (this_addr_ptr = server_info_ptr; this_addr_ptr != NULL; this_addr_ptr = this_addr_ptr->ai_next) {
        //TODO: add some cute printing
        socket_fd = socket(this_addr_ptr->ai_family, this_addr_ptr->ai_socktype, this_addr_ptr->ai_protocol);
        if (socket_fd == -1) {
            debugprintf("could not create socket: %s", strerror(errno));
            continue;
        } else {
            debugprintf("socket created");
        }

        if (connect(socket_fd, this_addr_ptr->ai_addr, this_addr_ptr->ai_addrlen) == -1) {
            debugprintf("could not connect socket to server: %s", strerror(errno));
            continue;
        } else {
            debugprintf("socket connected");
        }

        break;
    }

    if (this_addr_ptr == NULL) {
        fprintf(stderr, "failed to connect to server\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(server_info_ptr);

    //TODO: add cute printing about server

    // send filename to server
    memset(buf, 0, buf_size); // clear buffer
    strncpy(buf, filename, BUF_LEN - 1); // safely copy filename into buffer
    buf[BUF_LEN - 1] = '\0'; // ensure that buffer ends with null terminator
    if (send(socket_fd, buf, buf_size, 0) == -1) { //TODO: check send size?
        perror("error sending filename to server");
        exit(EXIT_FAILURE);
    }
    debugprintf("filename sent to server");

    exit(EXIT_SUCCESS);
}

void print_usage()
{
    printf("tcpclient is to be used in the following manner: \"tcpclient <HOSTNAME OR IP ADDRESS> <PORT> <FILENAME>\"\n");
}
