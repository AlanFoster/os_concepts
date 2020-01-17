#include "../drivers/screen.h"
#include "./idt.h"
#include <stdint.h>

extern void isr0();

void handle_isr(void) {
    print_string("error triggered!\n");
}

uint32_t pls(uint32_t handler) {
    uint16_t handler_address_low = (uint16_t) handler & 0xFFFF;
    uint16_t handler_address_high = (uint16_t) (handler >> 16) & 0xFFFF;

    return (handler_address_high << 16) | handler_address_low;
}

void install_isr() {
    print_string("Inside install isr\n");

    for (int i = 0 ; i < 256; i++) {
        set_idt_gate(i, (uint32_t) isr0);
    }

    load_idt();
}

