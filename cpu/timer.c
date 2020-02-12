#include "./timer.h"
#include "../drivers/ports.h"
#include "./isr.h"

#define TIMER_CHANNEL_0 0x40
#define TIMER_CHANNEL_1 0x41
#define TIMER_CHANNEL_2 0x42
#define TIMER_COMMAND_PORT 0x43

uint32_t tick = 0;

static void timer_callback(__attribute__((unused)) ISR_event e) {
    tick++;
}

uint32_t get_tick() {
    return tick;
}

void init_timer(uint32_t frequency) {
    uint16_t divisor = 1193180 / frequency;

    // Set PIT into periodic interrupt mode, i.e. repeating signal.
    port_byte_out(TIMER_COMMAND_PORT, 0x36);

    // Set low then high bit divisor
    port_byte_out(TIMER_CHANNEL_0, (uint8_t) (divisor & 0xFF));
    port_byte_out(TIMER_CHANNEL_0, (uint8_t) ((divisor >> 8) & 0xFF));

    // Register the interrupt handler for irq0, timer interrupts
    register_interrupt_handler(IRQ0, timer_callback);
}
