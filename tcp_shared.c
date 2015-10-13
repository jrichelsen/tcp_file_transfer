#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <mhash.h>
#include <inttypes.h>

#include "tcp_shared.h"

void analyze_argc( int argc, int argc_expected, void (* print_usage_ptr)() )
{
    if (argc == 1) {
        fprintf(stderr, "no command line arguments specified, exiting now.\n");
        print_usage_ptr();
        exit(EXIT_FAILURE);
    } else if (argc != argc_expected) {
        fprintf(stderr, "incorrect number of command line arguments, exiting now.\n");
        print_usage_ptr();
        exit(EXIT_FAILURE);
    }
}

int cmp_MD5_hash( unsigned char MD5_hash1[16], unsigned char MD5_hash2[16] )
{
    return memcmp(MD5_hash1, MD5_hash2, 16);
}

void debugprintf( const char * const format, ... )
{
    if (!DEBUG) {
        return;
    }

    va_list args;
    va_start(args, format);
    printf("DEBUG: ");
    vprintf (format, args);
    printf("\n");
}

void MD5_hash_of_byte_array( unsigned char * byteArray, size_t len, unsigned char * MD5_hash[16] )
{
    MHASH MD5_hash_context = mhash_init(MHASH_MD5);
    mhash(MD5_hash_context, byteArray, len);
    *MD5_hash = mhash_end(MD5_hash_context);
}

int open_filename_to_byte_array( char * filename, unsigned char * * byteArray )
{
    FILE * file = fopen(filename, "rb");
    if (file == NULL) {
        return -1;
    }
    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);
    fseek(file, 0, SEEK_SET);
    *byteArray = NULL;
    *byteArray = malloc(len + 1);
    if (*byteArray == NULL) {
        fprintf(stderr, "error allocating memory for file, exiting now\n");
        exit(EXIT_FAILURE);
    }
    fread(*byteArray, len, 1, file);
    fclose(file);
    (*byteArray)[len] = 0;
    return len;
}

void print_MD5_hash( unsigned char * MD5_hash[16] )
{
    int i_MD5_hash;
    for (i_MD5_hash = 0; i_MD5_hash < 16; i_MD5_hash++) {
        printf("%.2x", (*MD5_hash)[i_MD5_hash]);
    }
}
