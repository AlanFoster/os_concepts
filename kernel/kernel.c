#include "../drivers/screen.h"
#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "./util.h"

void main() {
    clear_screen();
    load_idt();
    isr_install();

    char str_buffer[20];

    print_string("hello world\n");

    for (int i = 0 ; i <= 10; i ++) {
        itoa(i, str_buffer);
        print_string(str_buffer);
        print_char(',');
        print_char('\n');
    }

    asm volatile ("int $0x9");
}
