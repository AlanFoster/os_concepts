#include "../drivers/screen.h"
#include "./util.h"

void main() {
    clear_screen();

    char str_buffer[20];

    for (int i = 0 ; i <= 200; i ++) {
        itoa(i, str_buffer);
        print_string(str_buffer);
        print_char(',');
        print_char('\n');
    }
}
