// Mitch Patin (mpatin)
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
// CONST POINTERS?

#include "tcp_shared.h" // utility functions

int DEBUG = 1; // flag for whether to print debug statements
#define MAX_PENDING 5
#define FILENAME_BUF_LEN 1000 // length (in characters) of buffer used to receive filename

void print_usage( ); // print correct command usage (arguments, etc.)

int main( int argc, char * argv[] )
{
    // variables and data structures
    const char * port_str; // (from command line)
    struct addrinfo hints; // hints for getaddrinfo()
    struct addrinfo * server_info_ll = NULL; // linked list returned by getaddrinfo()
    struct addrinfo * this_addr_ptr = NULL; // pointer to current item on server_info_ll linked list
    const int YES = 1; // used for setsockopt()
    int socket_fd; // socket accepting connections
    int client_socket_fd; // socket for communicating with client
    struct sockaddr_storage client_addr;
    socklen_t addr_len = sizeof(client_addr);
    unsigned short int filename_len_client; // length of filename sent from client
    char filename_buf[FILENAME_BUF_LEN];
    const size_t filename_buf_len = sizeof(filename_buf);
    unsigned char * byteArray = NULL; // byte array holding file to send to client
    ssize_t file_len; // length of file to send to client
    unsigned char * MD5_hash[16]; // pointer to array (NOT STRING) holding hex values for MD5 hash

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
    if ((gai_rv = getaddrinfo(NULL, port_str, &hints, &server_info_ll)) != 0) {
        fprintf(stderr, "error getting address information for server (self), quitting now: %s\n", gai_strerror(gai_rv));
        exit(EXIT_FAILURE);
    }

    // bind to first address where binding is possible
    for (this_addr_ptr = server_info_ll; this_addr_ptr != NULL; this_addr_ptr = this_addr_ptr->ai_next) {
        //TODO: add some cute printing
        // create connection socket
        socket_fd = socket(this_addr_ptr->ai_family, this_addr_ptr->ai_socktype, this_addr_ptr->ai_protocol);
        if (socket_fd == -1) {
            debugprintf("could not create connection socket: %s", strerror(errno));
            continue;
        } else {
            debugprintf("connection socket created");
        }

        // make connection socket reusable
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &YES, sizeof(const int)) == -1) {
            debugprintf("could not make socket reusable: %s", strerror(errno));
            continue;
        } else {
            debugprintf("connection socket set to reusable");
        }

        // bind connection socket
        if (bind(socket_fd, this_addr_ptr->ai_addr, this_addr_ptr->ai_addrlen) == -1) {
            debugprintf("could not bind to connection socket: %s", strerror(errno));
            continue;
        } else {
            debugprintf("connection socket bound");
        }

        break;
    }

    if (this_addr_ptr == NULL) {
        fprintf(stderr, "failed to bind server socket, exiting now\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(server_info_ll);

    if (listen(socket_fd, MAX_PENDING) == -1) {
        perror("failed to listen to socket, exiting now");
        exit(EXIT_FAILURE);
    }
    debugprintf("listening");

    client_socket_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_socket_fd == -1) {
        perror("failed to accept client connection, exiting now");
        exit(EXIT_FAILURE);
    }
    debugprintf("accepted client connection");

    //TODO: add cute printing about client

    // receive filename length from client
    uint16_t filename_len_client_net;
    int bytes_recvd_filename_len = recv(client_socket_fd, &filename_len_client_net, sizeof(uint16_t), 0);
    if (bytes_recvd_filename_len == -1) {
        perror("error receiving filename length from client, exiting now");
        exit(EXIT_FAILURE);
    }
    if (bytes_recvd_filename_len != sizeof(uint16_t)) {
        fprintf(stderr, "filename length of incorrect type, exiting now\n");
        exit(EXIT_FAILURE);
    }
    filename_len_client = ntohs(filename_len_client_net);
    debugprintf("filename length received from client: %hu", filename_len_client);

    // receive filename from client
    int bytes_recvd_filename = recv(client_socket_fd, filename_buf, (filename_buf_len - sizeof(char)), 0);
    if (bytes_recvd_filename == -1) {
        perror("error receiving filename from client, exiting now");
        exit(EXIT_FAILURE);
    }
    if (bytes_recvd_filename > filename_buf_len - sizeof(char)) {
        fprintf(stderr, "filename too large for filename buffer, exiting now\n");
        exit(EXIT_FAILURE);
    }
    filename_buf[FILENAME_BUF_LEN] = '\0';
    debugprintf("filename received from client: %s", filename_buf);

    // ensure that filename length and length of actual filename match
    if (filename_len_client != strlen(filename_buf)) {
        fprintf(stderr, "filename length and filename from client do not match, exiting now\n");
        exit(EXIT_FAILURE);
    }
    debugprintf("filename length and filename match");

    // attempt to open file on local filesystem
    file_len = open_filename_to_byte_array(filename_buf, &byteArray);
    debugprintf("file opened to byte array, %zu bytes", file_len);

    // send file length to client
    uint16_t file_len_net = htons(file_len);
    int bytes_sent_file_len = send(client_socket_fd, &file_len_net, sizeof(uint16_t), 0);
    if (bytes_sent_file_len == -1) {
        perror("error sending size of file to client, exiting now");
        exit(EXIT_FAILURE);
    }
    if (bytes_sent_file_len != (sizeof(file_len_net))) {
        fprintf(stderr, "error sending size of file to client: incorrect number of bytes sent, exiting now\n");
        exit(EXIT_FAILURE);
    }
    debugprintf("file length sent to client");

    // quit if file does not exist
    if (file_len == -1) {
        debugprintf("file does not exist, exiting now");
        exit(EXIT_SUCCESS);
    }

    // create MD5 hash of file
    MD5_hash_of_byte_array(byteArray, file_len, MD5_hash);
    debugprintf("MD5 hash created");

    // send MD5 hash to client
    int bytes_sent_MD5_hash = send(client_socket_fd, *MD5_hash, 16, 0);
    if (bytes_sent_MD5_hash == -1) {
        perror("error sending MD5 hash to client");
        exit(EXIT_FAILURE);
    }
    if (bytes_sent_MD5_hash != 16) {
        fprintf(stderr, "error sending MD5 hash to client: incorrect number of bytes sent\n");
        exit(EXIT_FAILURE);
    }
    debugprintf("MD5 hash sent to server");

    // send file byte array to client
    int bytes_sent_file = send(client_socket_fd, byteArray, file_len, 0);
    if (bytes_sent_file == -1) {
        perror("error sending file byte array to client");
        exit(EXIT_FAILURE);
    }
    if (bytes_sent_file != file_len) {
        fprintf(stderr, "error sending file byte array to client: incorrect number of bytes sent\n");
        exit(EXIT_FAILURE);
    }
    debugprintf("file sent to client, DONE");

    exit(EXIT_SUCCESS);
}

void print_usage( )
{
    printf("tcpserver is to be used in the following manner: \"tcpserver <PORT>\"\n");
}
