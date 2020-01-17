void memory_copy(char *dest, char *source, int nbytes) {
    for (int i = 0; i < nbytes; i++) {
        dest[i] = source[i];
    }
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

void memory_set(char *dest, char val, int amount) {
    for (int i = 0; i < amount; i++) {
        dest[i] = val;
    }
}
