#ifndef ISR_H_INCLUDED
#define ISR_H_INCLUDED

#include <stdint.h>

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

typedef struct {
    /**
     * The previous data segment descriptor
     */
    uint32_t datasegment;

    /**
     * The interrupt code, i.e. 0 for division by zero exception when a
     * cpu interrupt has been raised, or 32-47 for PIC interrupts.
     */
    uint32_t interrupt_code;

    /**
     * Error code if available. This may be a dummy value.
     */
    uint32_t error_code;

    // Additional values pushed by the processor automatically during an exception frame
    uint32_t eip;   // Return address
    uint32_t cs;    // code segment selector
    uint32_t eflags;
    uint32_t user_esp;
    uint32_t user_ss;
} ISR_event;

typedef void (*IRQ_Handler)(ISR_event);

void isr_install();
void register_interrupt_handler(uint8_t interrupt_code, IRQ_Handler handler);

#endif
