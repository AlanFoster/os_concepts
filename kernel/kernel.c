#include "../drivers/screen.h"
#include "../cpu/timer.h"
#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "./util.h"
#include "../drivers/keyboard.h"
#include "../drivers/ports.h"

#define SYSTEM_CLOCK_HZ 100

void print_help() {
    print_string("  ticks - print the total cpu ticks since boot\n");
    print_string("  halt - halt the machine\n");
    print_string("  help - print the available instructions\n");
}

void on_user_input(char *user_input) {
    if (strcmp(user_input, "ticks") == 0) {
        print_string("the total cpu ticks since boot is: %d\n", get_tick());
    } else if (strcmp(user_input, "halt") == 0) {
        print_string("halting...\n");
        asm volatile("hlt");
    } else if (strcmp(user_input, "help") == 0) {
        print_help();
    } else {
        print_string("unknown command\n");
        print_help();
    }
    print_string("> ");
}

void main() {
    clear_screen();
    load_idt();
    isr_install();
    init_timer(SYSTEM_CLOCK_HZ);
    init_keyboard();

    print_string("hello world!\n");

    for (int i = 0 ; i <= 8; i++) {
        print_string("int: %d - hex: %x\n", i, i);
    }

    print_string("> ");
}
