#include "util.h"
#include <stdint.h>
#include <stddef.h>

int atoi(char s[]) {
    int n = 0;
    for ( int i = 0 ; s[i] >= '0' && s[i] <= '9'; i++) {
        n *= 10;
        n += s[i] - '0';
    }
    return n;
}

int strlen(char str[]) {
    int i = 0;
    while (str[i] != '\0') {
        i++;
    }
    return i;
}

void reverse(char str[]) {
    int i = 0;
    int j = strlen(str);
    while ( i < j ) {
        char temp = str[i];
        str[i++] = str[--j];
        str[j] = temp;
    }
}

void itoa(int num, char str_buffer[]) {
    int i = 0;
    int sign = num < 0;

    if (sign) {
        num = -num;
    }

    // Create the string in reverse
    do {
        str_buffer[i++] = '0' + (num % 10);
        num /= 10;
    } while (num > 0);

    if (sign) {
        str_buffer[i++] = '-';
    }
    str_buffer[i++] = '\0';

    // Now reverse the string
    reverse(str_buffer);
}

void itohex(uint32_t num, char str_buffer[]) {
    int i = 0;

    // When num is 0, ensure 0x0 is returned
    if (num == 0) {
        str_buffer[i++] = '0';
    } else {
        while ( num != 0 ) {
            char ascii_value = (num & 0xF) + '0';

            // If the ascii value is > ascii 9, then add 7 to move to the a-f character range
            if (ascii_value > '9') {
                ascii_value += 7;
            }

            str_buffer[i++] = ascii_value;
            num >>= 4;
        }
    }

    str_buffer[i++] = 'x';
    str_buffer[i++] = '0';
    str_buffer[i++] = '\0';
    reverse(str_buffer);
}

void append(char str[], char c) {
    int len = strlen(str);
    str[len] = c;
    str[len + 1] = '\0';
}

int strcmp(char *str1, char *str2) {
    int i = 0;
    for ( i = 0 ; str1[i] == str2[i]; i++) {
        if (str1[i] == '\0') {
            return 0;
        }
    }

    return str1[i] - str2[i];
}

char *strncopy(char *destination, char *source, size_t maximum) {
  size_t i;
  for (i = 0; source[i] != '\0' && i < maximum; i++) {
    destination[i] = source[i];
  }
  destination[i] = '\0';
  return destination;
}