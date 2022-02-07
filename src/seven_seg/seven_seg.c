#include <msp430g2553.h>
#include "seven_seg.h"

struct {
    uint8_t                     clk;
    uint8_t                     dat[3];
    volatile unsigned char      *gpio_out;
    volatile unsigned char      *gpio_dir;
} seven_segs[SEVEN_SEG_COUNT];

int chars[][8] = {
//   H  G  F  E  D  C  B  A
    {0, 0, 0, 0, 0, 0, 0, 0}, // blank
    {0, 0, 1, 1, 1, 1, 1, 1}, // 0
    {0, 0, 0, 0, 0, 1, 1, 0}, // 1
    {0, 1, 0, 1, 1, 0, 1, 1}, // 2
    {0, 1, 0, 0, 1, 1, 1, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1, 0}, // 4
    {0, 1, 1, 0, 1, 1, 0, 1}, // 5
    {0, 1, 1, 1, 1, 1, 0, 1}, // 6
    {0, 0, 0, 0, 0, 1, 1, 1}, // 7
    {0, 1, 1, 1, 1, 1, 1, 1}, // 8
    {0, 1, 1, 0, 1, 1, 1, 1}, // 9
    // {0, 1, 1, 1, 0, 1, 1, 1}, // A
    // {0, 1, 1, 1, 1, 1, 0, 0}, // b
    // {0, 0, 1, 1, 1, 0, 0, 1}, // C
    // {0, 1, 0, 1, 1, 1, 1, 0}, // d
    // {0, 1, 1, 1, 1, 0, 0, 1}, // E
    // {0, 1, 1, 1, 0, 0, 0, 1}, // F
};

static void _setup_gpio(uint8_t id) {
    *(seven_segs[id].gpio_dir) |= seven_segs[id].clk | seven_segs[id].dat[0] | seven_segs[id].dat[1] | seven_segs[id].dat[2];
    *(seven_segs[id].gpio_out) &= ~(seven_segs[id].clk | seven_segs[id].dat[0] | seven_segs[id].dat[1] | seven_segs[id].dat[2]);
}

void seven_seg_write(uint8_t id, uint16_t val) {
    uint8_t dig, bit;
    uint8_t digs[3];

    digs[2] = val / 100;
    digs[1] = (val % 100) / 10;
    digs[0] = (val % 10) + 1;

    if (digs[2] != 0) digs[2] += 1;
    if (digs[2] != 0 || digs[1] != 0) digs[1] += 1;

    for (bit = 0; bit < 8; bit++) {

        for (dig = 0; dig < 3; dig++) {
            if (!chars[digs[dig]][bit]) *(seven_segs[id].gpio_out) |= seven_segs[id].dat[dig];
            else                        *(seven_segs[id].gpio_out) &= ~seven_segs[id].dat[dig];
        }

        asm("nop");
        *(seven_segs[id].gpio_out) |= seven_segs[id].clk;
        asm("nop");
        *(seven_segs[id].gpio_out) &= ~seven_segs[id].clk;
    }
}

void seven_seg_init(uint8_t id, uint8_t port, uint8_t clk, uint8_t dat0, uint8_t dat1, uint8_t dat2) {
    seven_segs[id].gpio_out = (port == 2) ? &P2OUT : &P1OUT;
    seven_segs[id].gpio_dir = (port == 2) ? &P2DIR : &P1DIR;
    seven_segs[id].clk = clk;
    seven_segs[id].dat[0] = dat0;
    seven_segs[id].dat[1] = dat1;
    seven_segs[id].dat[2] = dat2;

    _setup_gpio(id);
}
