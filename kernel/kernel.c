#include "../drivers/screen.h"
#include "../cpu/timer.h"
#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "./util.h"
#include "../drivers/keyboard.h"

#define SYSTEM_CLOCK_HZ 100

void main() {
    clear_screen();
    load_idt();
    isr_install();
    init_timer(SYSTEM_CLOCK_HZ);
    init_keyboard();

    print_string("hello world\n");

    for (int i = 0 ; i <= 32; i++) {
        print_string("int: %d - hex: %x\n", i, i);
    }
}
