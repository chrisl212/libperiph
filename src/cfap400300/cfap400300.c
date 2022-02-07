#include <msp430g2553.h>
#include <stdint.h>
#include "cfap400300.h"
#include "sd/sd.h"

#define NRST    (BIT0)
#define BSY     (BIT1)
#define MOSI    (BIT2)
#define DC      (BIT3)
#define SCK     (BIT4)
#define CS      (BIT0)

#define CMD     (0)
#define DATA    (1)

extern void delay(long cyc);
static void _spi_tx(uint8_t dc, uint8_t dat);
static void _wait_busy(void);
static void _setup_pins(void);
static void _setup_spi(void);
static void _reset_sequence(void);
static void _power_on_sequence(void);

void cfap400300_init(void) {
    _setup_pins();
    _setup_spi();
    _reset_sequence();
    _power_on_sequence();
}

static void _spi_tx(uint8_t dc, uint8_t dat) {
    if (dc) {
        P1OUT |= DC;
    } else {
        P1OUT &= ~DC;
    }

    P2OUT &= ~CS;
    UCA0TXBUF = dat;
    while (UCA0STAT & UCBUSY);

    P2OUT |= CS;
}

static void _wait_busy(void) {
    while (!(P1IN & BSY));
}

static void _setup_pins(void) {
    P1DIR       |= NRST + DC;
    P2DIR       |= CS;
    P1SEL       |= MOSI + SCK;
    P1SEL2      |= MOSI + SCK;

    P1OUT       |= NRST;
    P2OUT       |= CS;
}

static void _setup_spi(void) {
    UCA0CTL0    |= UCCKPH;      // DATA CAPTURED ON RISING EDGE
    UCA0CTL0    |= UCMSB;       // MSB FIRST
    UCA0CTL0    |= UCMST;       // MASTER MODE
    UCA0CTL0    |= UCMODE_0;    // 3-PIN SPI
    UCA0CTL0    |= UCSYNC;      // SYNCHRONOUS MODE
    UCA0CTL1    |= UCSSEL_2;    // SELECT SMCLK
    UCA0CTL1    &= ~UCSWRST;    // TURN ON
}

static void _reset_sequence(void) {
    uint8_t iter;

    for (iter = 0; iter < 3; iter++) {
        P1OUT &= ~NRST;
        delay(100);
        P1OUT |= NRST;
        delay(100);
    }
}

// BOOSTER SOFT START
// POWER SETTING
// POWER ON
// PANEL SETTING
// PLL CONTROL
// RESOLUTION SETTING
// LOAD DATA
// DISPLAY REFRESH
// BORDER FLOATING
static void _power_on_sequence(void) {
    uint16_t sector_idx;
    uint16_t sectors = 30;
    uint16_t byte_idx;
    uint8_t  byte;

    _spi_tx(CMD, PWR);
    _spi_tx(DATA, 0x03);
    _spi_tx(DATA, 0x00);
    _spi_tx(DATA, 0x2b);
    _spi_tx(DATA, 0x2b);

    _spi_tx(CMD, BTST);
    _spi_tx(DATA, 0x17); // DEFAULT VALUES
    _spi_tx(DATA, 0x17);
    _spi_tx(DATA, 0x17);

    _spi_tx(CMD, PON);

    _wait_busy();

    _spi_tx(CMD, PSR);
    _spi_tx(DATA, 0x13);

    _spi_tx(CMD, PLL);
    _spi_tx(DATA, 0x3a);

    _spi_tx(CMD, TRES);
    _spi_tx(DATA, 0x01);
    _spi_tx(DATA, 0x90);
    _spi_tx(DATA, 0x01);
    _spi_tx(DATA, 0x2c);

    _spi_tx(CMD, VDCS);
    _spi_tx(DATA, 0x12);

    _spi_tx(CMD, CDI);
    _spi_tx(DATA, 0x07);
    
    _spi_tx(CMD, PON);

    // _spi_tx(CMD, DST);
    _spi_tx(CMD, 0x13);

    for (sector_idx = 0; sector_idx < sectors; sector_idx++) {
        sd_sector_start_read(sector_idx);
        for (byte_idx = 0; byte_idx < 512; byte_idx++) {
            byte = sd_sector_read();
            if (sector_idx*512 + byte_idx < 15000) {
                _spi_tx(DATA, ~byte);
            }
        }
        sd_sector_finish_read();
    }
    // for (i = 0; i < 15000; i++) {
    //     if (i % 2 == 0) {
    //         _spi_tx(DATA, 0xff);
    //     } else {
    //         _spi_tx(DATA, 0x00);
    //     }
    // }

    // _spi_tx(CMD, DSP);

    // _spi_tx(CMD, IPC);
    // _spi_tx(DATA, 0x10);

    _spi_tx(CMD, DSR);

    _wait_busy();
    
    // _spi_tx(CMD, CDI);
    // _spi_tx(DATA, 0x07);

    _spi_tx(CMD, PWR_OFF);

    _spi_tx(CMD, PFS);
    _spi_tx(DATA, 0x00);

    // _spi_tx(CMD, DSLP);
    // _spi_tx(DATA, 0xa5);

    // DST1, OLD, DST2, NEW, DISPLAY_REFRESH, CHECK_BUSY, VCOM AND DATA INTERVAL (set border to floating), POWER OFF, DEEP SLEEP
}
