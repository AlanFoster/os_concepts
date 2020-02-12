#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <stdint.h>

/**
 * Initialize a timer that will be triggered at the given frequency.
 * This will be used for the system clock
 */
void init_timer(uint32_t frequency);
uint32_t get_tick();

#endif
