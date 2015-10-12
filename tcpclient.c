// Mitch Patin (mpatin)
// Tim Pusateri (???)
// Jon Richelsen (jrichels)
// CSE30264
// Programming Assignment 3: TCP
// TCP Client
// Due 2015-10-13

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>


#include "tcp_shared.h" // utility functions

int DEBUG = 1; // flag for whether to print debug statements
#define BUF_LEN 1000000 // length (in characters) of buffer used to receive and send messages

void print_usage( ); // print correct command usage (arguments, etc.)

int main( int argc, char * argv[] )
{
    // variables and data structures
    const char * server_hostname; // (from command line)
    const char * port_str; // (from command line)
    const char * filename; // (from command line)
    unsigned short int filename_len; // length of filename string
    struct addrinfo hints; // hints for getaddrinfo()
    struct addrinfo * server_info_ptr = NULL; // linked list returned by getaddrinfo()
    struct addrinfo * this_addr_ptr = NULL; // pointer to current item on server_info_ptr linked list
    int socket_fd; // socket for communicating with server
    unsigned char buf[BUF_LEN]; // buffer used to receive and send messages
    const size_t buf_len = sizeof(buf); // length of above buffer
    ssize_t file_len; // length of file sent by server

    // get information from command line
    analyze_argc(argc, 4, &print_usage);
    server_hostname = argv[1];
    debugprintf("server hostnamename argument: %s", server_hostname);
    port_str = argv[2];
    debugprintf("port argument: %s", port_str);
    filename = argv[3];
    debugprintf("filename argument: %s", filename);
    filename_len = strlen(filename);
    debugprintf("filename length: %hu", filename_len);

    // get address of server
    memset(&hints, 0, sizeof(hints)); // clear hints
    hints.ai_family = AF_UNSPEC; // return both IPv4 and IPv6 addresses
    hints.ai_socktype = SOCK_STREAM; // specify TCP

    int gai_rv;
    if ((gai_rv = getaddrinfo(server_hostname, port_str, &hints, &server_info_ptr)) != 0) {
        fprintf(stderr, "error getting address information for server, quitting now: %s\n", gai_strerror(gai_rv));
        exit(EXIT_FAILURE);
    }

    // connect to first address where connecting is possible
    for (this_addr_ptr = server_info_ptr; this_addr_ptr != NULL; this_addr_ptr = this_addr_ptr->ai_next) {
        //TODO: add some cute printing
        // create server socket
        socket_fd = socket(this_addr_ptr->ai_family, this_addr_ptr->ai_socktype, this_addr_ptr->ai_protocol);
        if (socket_fd == -1) {
            debugprintf("could not create server socket: %s", strerror(errno));
            continue;
        } else {
            debugprintf("server socket created");
        }

        // connect server socket to server
        if (connect(socket_fd, this_addr_ptr->ai_addr, this_addr_ptr->ai_addrlen) == -1) {
            debugprintf("could not connect server socket to server: %s", strerror(errno));
            continue;
        } else {
            debugprintf("server socket connected");
        }

        break;
    }

    if (this_addr_ptr == NULL) {
        fprintf(stderr, "failed to connect to server, exiting now\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(server_info_ptr);

    //TODO: add cute printing about server

    // send filename length to server
    uint16_t filename_len_net = htons(filename_len);
    int bytes_sent_filename_len = send(socket_fd, &filename_len_net, sizeof(uint16_t), 0);
    if (bytes_sent_filename_len == -1) {
        perror("error sending size of filename to server, exiting now");
        exit(EXIT_FAILURE);
    }
    if (bytes_sent_filename_len != (sizeof(filename_len_net))) {
        fprintf(stderr, "error sending size of filename to server: incorrect number of bytes sent, exiting now\n");
        exit(EXIT_FAILURE);
    }
    debugprintf("filename length sent to server: %d", filename_len);

    // send filename to server
    int bytes_sent_filename = send(socket_fd, filename, (filename_len + sizeof(char)), 0);
    if (bytes_sent_filename == -1) {
        perror("error sending filename to server");
        exit(EXIT_FAILURE);
    }
    if (bytes_sent_filename != (filename_len + sizeof(char))) {
        fprintf(stderr, "error sending filename to server: incorrect number of bytes sent\n");
        exit(EXIT_FAILURE);
    }
    debugprintf("filename sent to server: %s", filename);

    // receive file length from server
    uint16_t file_len_net;
    int bytes_recvd_file_len = recv(socket_fd, &file_len_net, sizeof(uint16_t), 0);
    if (bytes_recvd_file_len == -1) {
        perror("error receiving file length from client, exiting now");
        exit(EXIT_FAILURE);
    }
    if (bytes_recvd_file_len != sizeof(uint16_t)) {
        fprintf(stderr, "file length of incorrect type, exiting now\n");
        fprintf(stderr, "%d\n", bytes_recvd_file_len);
        exit(EXIT_FAILURE);
    }
    file_len = ntohs(file_len_net);
    debugprintf("file length received from server: %zd", file_len);

    exit(EXIT_SUCCESS);
}

void print_usage( )
{
    printf("tcpclient is to be used in the following manner: \"tcpclient <HOSTNAME OR IP ADDRESS> <PORT> <FILENAME>\"\n");
}
