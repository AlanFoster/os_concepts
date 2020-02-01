#include "../drivers/screen.h"
#include "./isr.h"
#include "./idt.h"
#include "../drivers/ports.h"
#include <stdint.h>

#define MASTER_COMMAND 0x20
#define MASTER_DATA (MASTER_COMMAND + 1)
#define SLAVE_COMMAND 0xA0
#define SLAVE_DATA (SLAVE_COMMAND + 1)
#define TOTAL_HANDLERS 256

IRQ_Handler interrupt_handlers[TOTAL_HANDLERS];

// Error isrs
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

// PIC interrupts
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void register_error_handling(void);
void remap_pic_irq(void);
void register_pic_irqs(void);

void handle_isr(ISR_event e) {
    print_string("isr triggered:\n");
    print_string("error code was: %d\n", e.error_code);
    print_string("interrupt code was: %d\n", e.interrupt_code);

    for (;;);
}

void handle_irq(ISR_event e) {
    // Send an EOI (End of interrupt) signal to master and slave
    // PICs as required.
    if (e.interrupt_code >= 40) {
        port_byte_out(SLAVE_COMMAND, 0x20);
    }
    port_byte_out(MASTER_COMMAND, 0x20);

    // If a handler is present, trigger it
    IRQ_Handler handler = interrupt_handlers[e.interrupt_code];
    if (handler != 0) {
        interrupt_handlers[e.interrupt_code](e);
    }
}

void isr_install(void) {
    memory_set(interrupt_handlers, 0, TOTAL_HANDLERS);
    register_error_handling();
    register_pic_irqs();

    // Remember to set the interrupt flag, enabling interrupts
    asm volatile("sti");
}

void register_error_handling(void) {
    set_idt_gate(0, (uint32_t) isr0);
    set_idt_gate(1, (uint32_t) isr1);
    set_idt_gate(2, (uint32_t) isr2);
    set_idt_gate(3, (uint32_t) isr3);
    set_idt_gate(4, (uint32_t) isr4);
    set_idt_gate(5, (uint32_t) isr5);
    set_idt_gate(6, (uint32_t) isr6);
    set_idt_gate(7, (uint32_t) isr7);
    set_idt_gate(8, (uint32_t) isr8);
    set_idt_gate(9, (uint32_t) isr9);
    set_idt_gate(10, (uint32_t) isr10);
    set_idt_gate(11, (uint32_t) isr11);
    set_idt_gate(12, (uint32_t) isr12);
    set_idt_gate(13, (uint32_t) isr13);
    set_idt_gate(14, (uint32_t) isr14);
    set_idt_gate(15, (uint32_t) isr15);
    set_idt_gate(16, (uint32_t) isr16);
    set_idt_gate(17, (uint32_t) isr17);
    set_idt_gate(18, (uint32_t) isr18);
    set_idt_gate(19, (uint32_t) isr19);
    set_idt_gate(20, (uint32_t) isr20);
    set_idt_gate(21, (uint32_t) isr21);
    set_idt_gate(22, (uint32_t) isr22);
    set_idt_gate(23, (uint32_t) isr23);
    set_idt_gate(24, (uint32_t) isr24);
    set_idt_gate(25, (uint32_t) isr25);
    set_idt_gate(26, (uint32_t) isr26);
    set_idt_gate(27, (uint32_t) isr27);
    set_idt_gate(28, (uint32_t) isr28);
    set_idt_gate(29, (uint32_t) isr29);
    set_idt_gate(30, (uint32_t) isr30);
    set_idt_gate(31, (uint32_t) isr31);
}

void remap_pic_irq(void) {
    // By default:
    // Master IRQs 0-7  are mapped to INT 0x8-0xF
    // Slave IRQs 8-15 are mapped to int 0x70-0x77
    // These IRQs conflict with the error handling ISRs, so let's map
    // them to 'free' ISR slots that aren't assocciated with error
    // handling. Remapping PICs is done via ports.

    // Initialize master + slave PIC
    port_byte_out(MASTER_COMMAND, 0x11);
    port_byte_out(SLAVE_DATA, 0x11);

    // Remap IRQs to higher, 'free' slots
    port_byte_out(MASTER_DATA, 0x20);
    port_byte_out(SLAVE_DATA, 0x28);
    port_byte_out(MASTER_DATA, 0x04);
    port_byte_out(SLAVE_DATA, 0x02);
    port_byte_out(MASTER_DATA, 0x01);
    port_byte_out(SLAVE_DATA, 0x01);
    port_byte_out(MASTER_DATA, 0x0);
    port_byte_out(SLAVE_DATA, 0x0);
}

void register_pic_irqs(void) {
    remap_pic_irq();

    // Register handler
    set_idt_gate(32, (uint32_t) irq0);
    set_idt_gate(33, (uint32_t) irq1);
    set_idt_gate(34, (uint32_t) irq2);
    set_idt_gate(35, (uint32_t) irq3);
    set_idt_gate(36, (uint32_t) irq4);
    set_idt_gate(37, (uint32_t) irq5);
    set_idt_gate(38, (uint32_t) irq6);
    set_idt_gate(39, (uint32_t) irq7);
    set_idt_gate(40, (uint32_t) irq8);
    set_idt_gate(41, (uint32_t) irq9);
    set_idt_gate(42, (uint32_t) irq10);
    set_idt_gate(43, (uint32_t) irq11);
    set_idt_gate(44, (uint32_t) irq12);
    set_idt_gate(45, (uint32_t) irq13);
    set_idt_gate(46, (uint32_t) irq14);
    set_idt_gate(47, (uint32_t) irq15);
}

void register_interrupt_handler(uint8_t interrupt_code, IRQ_Handler handler) {
    interrupt_handlers[interrupt_code] = handler;
}
