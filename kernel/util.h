#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdint.h>

void itoa(int n, char target[]);
void itohex(uint32_t n, char target[]);
int atoi(char s[]);
void reverse(char str[]);
int strlen(char str[]);
void append(char str[], char c);
int strcmp(char *str1, char *str2);

#endif
