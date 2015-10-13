// Mitch Patin (???)
// Tim Pusateri (???)
// Jon Richelsen (jrichels)
// CSE30264
// Programming Assignment 3: TCP
// Shared Functions
// Due 2015-10-13

#ifndef TCP_SHARED_H
#define TCP_SHARED_H

extern int DEBUG; // flag for whether to print debug statements

void analyze_argc( int argc, int argc_expected, void (* print_usage_ptr)() );
int cmp_MD5_hash( unsigned char MD5_hash1[16], unsigned char MD5_hash2[16] );
void debugprintf( const char * const format, ... );
void MD5_hash_of_byte_array( unsigned char * byteArray, size_t len, unsigned char * MD5_hash[16] );
int open_filename_to_byte_array( char * filename, unsigned char * * byteArray );
void print_MD5_hash( unsigned char * MD5_hash[16] );

#endif //TCP_SHARED_H
