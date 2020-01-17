#include "../drivers/screen.h"
#include "../cpu/idt.h"
#include "./util.h"

void main() {
    clear_screen();
    install_isr();

    char str_buffer[20];

    print_string("hello world\n");

    for (int i = 0 ; i <= 10; i ++) {
        itoa(i, str_buffer);
        print_string(str_buffer);
        print_char(',');
        print_char('\n');
    }

    // asm volatile ("int $0x3");
    // asm volatile ("int $0x3");
}
