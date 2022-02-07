#ifndef RCWL1601_H
#define RCWL1601_H

#include <stdint.h>

#define RCWL1601_SAMPLES        (8)
#define RCWL1601_SAMPLES_BITS   (3)

typedef void (*rcwl1601_callback_t)(uint16_t);

void rcwl1601_init(uint8_t timer, uint8_t port, uint8_t trig, uint8_t echo, rcwl1601_callback_t callback);
void rcwl1601_timer_interrupt(void);

#endif
