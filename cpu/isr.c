#include "../drivers/screen.h"
#include "./isr.h"
#include <stdint.h>

extern void isr0();

void handle_isr(ISR_event e) {
    print_string("error code was %d\n", e.error_code);
    print_string("interrupt code was %d\n", e.interrupt_code);

    for (;;);
}

void install_isr() {
    print_string("Inside install isr\n");

    for (int i = 0 ; i < 256; i++) {
        set_idt_gate(i, (uint32_t) isr0);
    }

    load_idt();

    int x = 0 / 0;
    // asm volatile ("int $0x7");
}

