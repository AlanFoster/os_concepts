#ifndef IDT_H_INCLUDED
#define IDT_H_INCLUDED

#include <stdint.h>
#define IDT_ENTRIES 256
#define KERNEL_CODE_SEGMENT 0x08

// The IDT (Interupt dispatch table) registers handlers for 256 interrupts
// An interupt request (IRQ) is triggered when particular events happen, such as division
// by zero, or keyboard interupt requests.
//
// This table most be loaded with the lidt instruction

typedef struct {
    /**
     * Lower bits of the handler's function address location 0..15
     */
    uint16_t handler_address_low;

    /**
     * Code segment selector in GDT
     */
    uint16_t kernel_segment_selector;

    /**
     * Unused, always set as zero
     */
    uint8_t always_zero;

    /**
     * Type flags.
     * Bit 7: 1 if an interrupt is present, 0 otherwise
     * bit 6-5: Privilege level, 2 bit binary number for the minimum priviledge
     *          level the calling descriptor should have.
     * Bit 4: Segment storage, set to 0 for interupt and trap gates
     * bits 3-0: Gate type.
     *          0b1110 = 32-bit interrupt gate
     *          0b1111 = 32-bit trap gates
     */
    uint8_t type_flags;
    /**
     * Highest bits of the handler's address 16..31
     */
    uint16_t handler_address_high;
} __attribute__((packed)) IDT_entry;

/**
 * IDT pointer, similar to the GDT pointer defined within bootloader.asm
 */
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) IDT_pointer;

IDT_entry idt[IDT_ENTRIES];
IDT_pointer idt_pointer;

/**
 * Set a handler for the given idt entry
 **/
void set_idt_gate(int n, uint32_t handler);

void load_idt();

#endif
