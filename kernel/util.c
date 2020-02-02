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

void itohex(int num, char str_buffer[]) {
    int i = 6;

    str_buffer[0] = '0';
    str_buffer[1] = 'x';
    str_buffer[i--] = '\0';

    for (; i > 1; i--) {
        char ascii_value = (num & 0xF) + '0';

        // If the ascii value is > ascii 9, then add 7 to move to the a-f character range
        if (ascii_value > '9') {
            ascii_value += 7;
        }

        str_buffer[i] = ascii_value;
        num >>= 4;
    }
}

void memory_set(char *dest, char val, int amount) {
    for (int i = 0; i < amount; i++) {
        dest[i] = val;
    }
}
