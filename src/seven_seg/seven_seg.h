#ifndef SEVEN_SEG_H
#define SEVEN_SEG_H

#include <stdint.h>

#define SEVEN_SEG_COUNT (1)

void seven_seg_init(uint8_t id, uint8_t port, uint8_t clk, uint8_t dat0, uint8_t dat1, uint8_t dat2);
void seven_seg_write(uint8_t id, uint16_t val);

#endif
