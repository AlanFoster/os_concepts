#include <stdint.h>
#include "idt.h"
#include "../kernel/util.h"

void load_idt() {
    idt_pointer.base = (uint32_t) &idt;
    idt_pointer.limit = IDT_ENTRIES * sizeof(struct idt_entry) - 1;

    __asm__ __volatile__("lidtl (%0)" : : "r" (&idt_pointer));
}

void set_idt_gate(int n, uint32_t handler) {
    idt[n].handler_address_low = (uint16_t) handler & 0xFFFF;
    idt[n].kernel_segment_selector = KERNEL_CODE_SEGMENT;
    idt[n].always_zero = 0;
    idt[n].type_flags = 0x8E;
    idt[n].handler_address_high = (uint16_t) (handler >> 16) & 0xFFFF;
}
