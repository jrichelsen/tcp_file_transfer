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
void debugprintf( const char * const format, ... );
void md5HashStringOfByteArray( unsigned char * byteArray, size_t len, char * * hash_str );
ssize_t openFileToByteArray( char * filename, unsigned char * * byteArray );

#endif //TCP_SHARED_H
