#include "../drivers/screen.h"
#include "../cpu/timer.h"
#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "./util.h"

#define SYSTEM_CLOCK_HZ 100


void main() {
    clear_screen();
    load_idt();
    isr_install();
    init_timer(SYSTEM_CLOCK_HZ);

    char str_buffer[20];

    print_string("hello world\n");

    for (int i = 0 ; i <= 10; i ++) {
        itoa(i, str_buffer);
        print_string(str_buffer);
        print_char(',');
        print_char('\n');
    }
}
