// Mitch Patin (???)
// Tim Pusateri (???)
// Jon Richelsen (jrichels)
// CSE30264
// Programming Assignment 3: TCP
// Shared Functions
// Due 2015-10-13

#ifndef TCP_SHARED_H
#define TCP_SHARED_H

#include <stdint.h> // uint16_t (port type)

extern int DEBUG;

void debugprintf(const char * const format, ...);
uint16_t port_string_to_uint16_t(const char * const string_value, void (* print_usage_ptr)());
void analyze_argc(int argc, int argc_expected, void (* print_usage_ptr)());

#endif //TCP_SHARED_H
