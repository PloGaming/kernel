#ifndef STRING_H
#define STRING_H
#include <stdbool.h>

int strnlen(const char* str, int max);
int strlen(const char* ptr);
bool isDigit(char c);
int to_numeric_digit(char c);

#endif