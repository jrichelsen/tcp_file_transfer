// Mitch Patin (???)
// Tim Pusateri (???)
// Jon Richelsen (jrichels)
// CSE30264
// Programming Assignment 3: TCP
// TCP Server
// Due 2015-10-13

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>

#include "tcp_shared.h" // utility functions

int DEBUG = 1; // flag for whether to print debug statements
#define MAX_PENDING 5
#define BUF_LEN 1000000 // length (in characters) of buffer used to receive and send messages
#define FILENAME_BUF_LEN 1000 // length (in characters) of buffer used to store filename

void print_usage(); // print correct command usage (arguments, etc.)

int main( int argc, char * argv[] )
{
    // variables and data structures
    const char * port_str; // socket (from command line)
    struct addrinfo hints; // hints for getaddrinfo()
    struct addrinfo * server_info_ptr; // linked list returned by getaddrinfo()
    struct addrinfo * this_addr_ptr; // pointer to current item on server_info_ptr linked list
    const int YES = 1; // used for setsockopt()
    int socket_fd; // socket accepting connections
    int client_socket_fd; // socket for communicating with client
    struct sockaddr_storage client_addr; // client's address
    socklen_t addr_size = sizeof(client_addr);
    char buf[BUF_LEN]; // buffer used to receive and send messages
    size_t buf_size = sizeof(buf); // size of above buffer
    char filename_buf[FILENAME_BUF_LEN]; // buffer to store filename from client

    // get information from command line
    analyze_argc(argc, 2, &print_usage);
    port_str = argv[1];
    debugprintf("port argument: %s", port_str);

    // get address of server (self)
    memset(&hints, 0, sizeof(hints)); // clear hints
    hints.ai_flags = AI_PASSIVE; // autofill IP address
    hints.ai_family = AF_UNSPEC; // return both IPv4 and IPv6 addresses
    hints.ai_socktype = SOCK_STREAM; // specify TCP

    int gai_rv;
    if ((gai_rv = getaddrinfo(NULL, port_str, &hints, &server_info_ptr)) != 0) {
        fprintf(stderr, "error getting address information for server (self): %s\n", gai_strerror(gai_rv));
        exit(EXIT_FAILURE);
    }

    // bind to first address where binding is possible
    for (this_addr_ptr = server_info_ptr; this_addr_ptr != NULL; this_addr_ptr = this_addr_ptr->ai_next) {
        //TODO: add some cute printing
        socket_fd = socket(this_addr_ptr->ai_family, this_addr_ptr->ai_socktype, this_addr_ptr->ai_protocol);
        if (socket_fd == -1) {
            debugprintf("could not create socket: %s", strerror(errno));
            continue;
        } else {
            debugprintf("socket created");
        }

        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &YES, sizeof(const int)) == -1) {
            debugprintf("could not make socket reusable: %s", strerror(errno));
            continue;
        } else {
            debugprintf("socket set to reusable");
        }

        if (bind(socket_fd, this_addr_ptr->ai_addr, this_addr_ptr->ai_addrlen) == -1) {
            debugprintf("could not bind to socket: %s", strerror(errno));
            continue;
        } else {
            debugprintf("socket bound");
        }

        break;
    }

    if (this_addr_ptr == NULL) {
        fprintf(stderr, "failed to bind server\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(server_info_ptr);

    if (listen(socket_fd, MAX_PENDING) == -1) {
        perror("failed to listen to socket");
        exit(EXIT_FAILURE);
    }
    debugprintf("listening");

    client_socket_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &addr_size);
    if (client_socket_fd == -1) {
        perror("failed to accept client");
        exit(EXIT_FAILURE);
    }
    debugprintf("accepted connection");

    //TODO: add cute printing about client

    // receive filename from client
    memset(buf, 0, buf_size); // clear buffer
    if (recv(client_socket_fd, buf, buf_size, 0) == -1) { //TODO: can we use length for anything?
        perror("error receiving filename from client");
        exit(EXIT_FAILURE);
    }
    if (strlen(buf) > (FILENAME_BUF_LEN - 1)) {
        fprintf(stderr, "filename too large for filename buffer\n");
        exit(EXIT_FAILURE);
    }
    strncpy(filename_buf, buf, strlen(buf) + 1);
    debugprintf("filename received from client: %s", filename_buf);

    exit(EXIT_SUCCESS);
}

void print_usage()
{
    printf("tcpserver is to be used in the following manner: \"tcpserver <PORT>\"\n");
}
