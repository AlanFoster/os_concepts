#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <stddef.h>

void memory_copy(char *dest, char *source, int nbytes);
void memory_set(char *dest, int val, size_t amount);
uint32_t kmalloc(uint32_t required_size, int align_to_4KB);

#endif
