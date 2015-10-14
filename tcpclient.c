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
#include <sys/time.h>

#include "tcp_shared.h" // utility functions

int DEBUG = 1; // flag for whether to print debug statements

void print_usage( ); // print correct command usage (arguments, etc.)

int main( int argc, char * argv[] )
{
    // variables and data structures
    const char * server_hostname; // (from command line)
    const char * port_str; // (from command line)
    const char * filename; // (from command line)
    unsigned short int filename_len; // length of filename string
    struct addrinfo hints; // hints for getaddrinfo()
    struct addrinfo * server_info_ll = NULL; // linked list returned by getaddrinfo()
    struct addrinfo * this_addr_ptr = NULL; // pointer to current item on server_info_ll linked list
    int socket_fd; // socket for communicating with server
    ssize_t file_len; // length of file sent by server
    unsigned char MD5_hash_server[16]; // array (NOT STRING) holding hex values for MD5 hash from server
    unsigned char * file_buf = NULL; // buffer used to receive file
    unsigned char * MD5_hash_client[16]; // pointer to array (NOT STRING) holding hex values for MD5 hash from client (self)
    FILE * file = NULL; // file received from client
	struct timeval timestart; // capture time start
	struct timeval timeend;	// capture time end

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

	// capture start time
	gettimeofday(&timestart, NULL);

    // get address of server
    memset(&hints, 0, sizeof(hints)); // clear hints
    hints.ai_family = AF_UNSPEC; // return both IPv4 and IPv6 addresses
    hints.ai_socktype = SOCK_STREAM; // specify TCP

    int gai_rv;
    if ((gai_rv = getaddrinfo(server_hostname, port_str, &hints, &server_info_ll)) != 0) {
        fprintf(stderr, "error getting address information for server, quitting now: %s\n", gai_strerror(gai_rv));
        exit(EXIT_FAILURE);
    }

    // connect to first address where connecting is possible
    for (this_addr_ptr = server_info_ll; this_addr_ptr != NULL; this_addr_ptr = this_addr_ptr->ai_next) {
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

    freeaddrinfo(server_info_ll);

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
        fprintf(stderr, "file length of incorrect size, exiting now\n");
        exit(EXIT_FAILURE);
    }
    file_len = (ssize_t)ntohs(file_len_net);
    debugprintf("file length received from server: %zd", file_len);

    // quit if file does not exist on server
    if (file_len == -1) {
        fprintf(stderr, "File does not exists\n");
        exit(EXIT_SUCCESS);
    }

    // receive MD5 hash from server
    int bytes_recvd_MD5_hash = recv(socket_fd, MD5_hash_server, 16, 0);
    if (bytes_recvd_MD5_hash == -1) {
        perror("error receiving MD5 hash from server, exiting now");
        exit(EXIT_FAILURE);
    }
    if (bytes_recvd_MD5_hash != 16) {
        fprintf(stderr, "MD5 hash of incorrect size, exiting now\n");
        exit(EXIT_FAILURE);
    }
    debugprintf("MD5 hash received from server");

    // receive file byte array from server
    file_buf = (unsigned char *)malloc(file_len * sizeof(unsigned char *));
    int bytes_recvd_file = recv(socket_fd, file_buf, file_len, 0);
    if (bytes_recvd_file == -1) {
        perror("error receiving file byte array from server, exiting now");
        exit(EXIT_FAILURE);
    }
    if (bytes_recvd_file != file_len) {
        fprintf(stderr, "file byte array of incorrect size, exiting now: %d\n", bytes_recvd_file);
        exit(EXIT_FAILURE);
    }
    debugprintf("file received from server");

    // create MD5 hash of file
    MD5_hash_of_byte_array(file_buf, file_len, MD5_hash_client);
    debugprintf("MD5 hash created");

    // compare MD5 hashes
    if (cmp_MD5_hash(*MD5_hash_client, MD5_hash_server) != 0) {
        fprintf(stderr, "File hashes do not match – bad transfer\n");
        exit(EXIT_FAILURE);
    }
    debugprintf("MD5 hashes match"); //TODO: 

    // write byte array to file
    file = fopen("newfile", "wb");
    fwrite(file_buf, 1, file_len, file); //return value!
    debugprintf("file created, DONE");

	// capture end time
	gettimeofday(&timeend, NULL);

	// calculate and print time difference and throughput
	long diff_msec = timeend.tv_usec - timestart.tv_usec;
	printf("%d bytes transferred in %lu microseconds. Throughput: %lu Megabytes/sec.\n", 0, diff_msec, 0.0);

    exit(EXIT_SUCCESS);
}

void print_usage( )
{
    printf("tcpclient is to be used in the following manner: \"tcpclient <HOSTNAME OR IP ADDRESS> <PORT> <FILENAME>\"\n");
}
