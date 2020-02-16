#include "mem.h"
#include <stdint.h>
#define DEFAULT_KMALLOC_ADDR_POINTER 0x10000

void memory_copy(char *dest, char *source, int nbytes) {
    for (int i = 0; i < nbytes; i++) {
        dest[i] = source[i];
    }
}

void memory_set(char *dest, char val, int amount) {
    for (int i = 0; i < amount; i++) {
        dest[i] = val;
    }
}

/**
 * Pointer to the next free location that kmalloc will return.
 * This should be computed at link time, for now its an arbitrary
 * location after our OS - which is currently linked to 0x1000.
 */
static uint32_t kmalloc_addr_pointer = DEFAULT_KMALLOC_ADDR_POINTER;

uint32_t kmalloc(uint32_t required_size, int align_to_4KB, uint32_t *physical_address) {
    // TODO: There's something funky going on, kmalloc_addr_pointer is 0 on boot... Let's set it ourselves.
    if (kmalloc_addr_pointer == 0) {
        kmalloc_addr_pointer = DEFAULT_KMALLOC_ADDR_POINTER;
    }

    // If the address isn't correctly aligned to 4kb, i.e. for pages, correct this.
    if (align_to_4KB == 1 && (kmalloc_addr_pointer & 0xFFFFF000)) {
        kmalloc_addr_pointer &= 0xFFFFF000;
        kmalloc_addr_pointer += 0x1000;
    }

    uint32_t assigned_memory_location = kmalloc_addr_pointer;
    if (physical_address) {
        *physical_address = assigned_memory_location;
    }

    kmalloc_addr_pointer += required_size;
    return assigned_memory_location;
}
