#ifndef ISR_H_INCLUDED
#define ISR_H_INCLUDED

#include <stdint.h>

typedef struct {
    uint32_t datasegment;
    // Pushed registers, from pusha within interrupts.asm
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    /**
     * The interrupt code, i.e. 0 for division by zero exception
     */
    uint32_t interrupt_code;
    /**
     * Error code if available. This may be a dummy value.
     */
    uint32_t interrupt_code;
    // Additional values pushed by the processor
    uint32_t eip;   // Return address
    uint32_t cs;    // code segment selector
    uint32_t eflags;
    uint32_t user_esp;
    uint32_t user_ss;
} ISR_event;

void isr_install();
// void isr_handler(ISR_event e);
void isr_handler();

#endif
