#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <mhash.h>

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

void debugprintf( const char * const format, ... )
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

void md5HashStringOfByteArray( unsigned char * byteArray, size_t len, char * * hash_str )
{
    MHASH hash_context = mhash_init(MHASH_MD5);
    mhash(hash_context, byteArray, len);
    unsigned char * hash_hex = mhash_end(hash_context);

    int i_hash_char;
    for (i_hash_char = 0; i_hash_char < mhash_get_block_size(MHASH_MD5); i_hash_char ++) {
        sprintf(hash_str[i_hash_char], "%x", hash_hex[i_hash_char]);
    }
}

ssize_t openFileToByteArray( char * filename, unsigned char * * byteArray )
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
