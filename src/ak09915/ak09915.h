#ifndef AK09915_H
#define AK09915_H

#include <stdint.h>

#define AK09915_USCIA
// #define AK09915_USCIB
#define AK09915_PORT1
// #define AK09915_PORT2

typedef void (*ak09915_callback_t)(int16_t, int16_t, int16_t);

void ak09915_init(uint8_t usci, uint8_t port, uint8_t cs, uint8_t sck, uint8_t miso, uint8_t mosi, uint8_t interrupt, ak09915_callback_t callback);

#endif
