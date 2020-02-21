#include "../drivers/screen.h"
#include "../cpu/timer.h"
#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "./util.h"
#include "../drivers/keyboard.h"
#include "../drivers/ports.h"
#include "./mem.h"
#include "./paging.h"

#define SYSTEM_CLOCK_HZ 100
#define RED "\e[31m"
#define GREEN "\e[32m"
#define YELLOW "\e[33m"
#define CYAN "\e[36m"
#define RESET "\e[0m"

void print_help() {
    print_string("  " GREEN "ticks" RESET " - print the total cpu ticks since boot\n");
    print_string("  " GREEN "count" RESET " - count in hex\n");
    print_string("  " GREEN "kmalloc" RESET " - test kmalloc\n");
    print_string("  " GREEN "halt" RESET " - halt the machine\n");
    print_string("  " GREEN "help" RESET " - print the available instructions\n");
}


void test_kmalloc() {
    uint32_t allocated_memory = kmalloc(0, 1);
    print_string("Allocated memory: %x\n", allocated_memory);
}

void on_user_input(char *user_input) {
    if (strcmp(user_input, "ticks") == 0) {
        print_string("the total cpu ticks since boot is: " YELLOW "%d" RESET "\n", get_tick());
    } else if (strcmp(user_input, "halt") == 0) {
        print_string("halting...\n");
        asm volatile("hlt");
    } else if (strcmp(user_input, "help") == 0) {
        print_help();
    } else if (strcmp(user_input, "kmalloc") == 0) {
        test_kmalloc();
    } else if (strcmp(user_input, "count") == 0) {
        for (int i = 0 ; i <= 32; i++) {
            print_string("int: %d - hex: " YELLOW "%x" RESET "\n", i, i);
        }
    } else {
        print_string(RED "unknown command\n" RESET);
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
    init_paging();

    print_string("type " CYAN "help" RESET " to see the available commands\n");
    print_string("> ");
}
