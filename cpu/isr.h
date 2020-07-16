#ifndef ISR_H_INCLUDED
#define ISR_H_INCLUDED

#include <stdint.h>

#define DIVISION_BY_ZERO_EXCEPTION 0
#define DEBUG_EXCEPTION 1
#define NON_MASKABLE_INTERRUPT_EXCEPTION 2
#define BREAKPOINT_EXCEPTION 3,
#define INTO_DETECTED_OVERFLOW_EXCEPTION 4
#define OUT_OF_BOUNDS_EXCEPTION 5
#define INVALID_OPCODE_EXCEPTION 6
#define NO_COPROCESSOR_EXCEPTION 7
#define DOUBLE_FAULT_EXCEPTION 8
#define COPROCESSOR_SEGMENT_OVERRUN_EXCEPTION 9
#define BAD_TSS_EXCEPTION 10
#define SEGMENT_NOT_PRESENT_EXCEPTION 11
#define STACK_FAULT_EXCEPTION 12
#define GENERAL_PROTECTION_FAULT_EXCEPTION 13
#define PAGE_FAULT_EXCEPTION 14
#define UNKNOWN_INTERRUPT_EXCEPTION 15,
#define COPROCESSOR_FAULT_EXCEPTION 16
#define ALIGNMENT_CHECK_EXCEPTION_486_PLUS 17
#define MACHINE_CHECK_EXCEPTION_PENTIUM_586_PLUS = 18

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

struct interrupt_event {
    /**
     * The previous data segment descriptor
     */
    uint32_t datasegment;

    /**
     * The cr2 control register
     */
    uint32_t cr2;

    /**
     * The old register state before the interrupt occurred
     */
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;

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
};

typedef void (*interrupt_handler)(struct interrupt_event);

void isr_install();
void register_interrupt_handler(uint8_t interrupt_code, interrupt_handler handler);

#endif
