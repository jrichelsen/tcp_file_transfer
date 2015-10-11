// Mitch Patin (???)
// Tim Pusateri (???)
// Jon Richelsen (jrichels)
// CSE30264
// Programming Assignment 3: TCP
// Shared Functions
// Due 2015-10-13

#ifndef TCP_SHARED_H
#define TCP_SHARED_H

void debugprintf(const char * const format, ...);
long int port_string_to_long_int(const char * const string_value, void (* print_usage_ptr)());
void analyze_argc(int argc, int argc_expected, void (* print_usage_ptr)());

#endif //TCP_SHARED_H
