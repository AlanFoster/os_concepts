/**
 * Reading a byte from the given port
 */
unsigned char port_byte_in(unsigned short port) {
    unsigned char result;
    // Note, __asm__ is using GNU's GAS syntax
    //   https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html
    // "=a" (result) - Place AL register into result
    // "d" (port) - Put port into EDX register
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void port_byte_out(unsigned short port, unsigned char data) {
    // "a" (data) - Put data in EAX
    // "d" (port) - Put port in EDX
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

unsigned short port_word_in(unsigned short port) {
    unsigned short result;
    // Note, __asm__ is using GNU's GAS syntax
    // "=a" (result) - Place AL register into result
    // "d" (port) - Put port into EDX register
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void port_word_out(unsigned short port, unsigned short data) {
    // "a" (data) - Put data in EAX
    // "d" (port) - Put port in EDX
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}
