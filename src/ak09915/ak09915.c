#include <msp430g2553.h>
#include "ak09915.h"
#include "ak09915_defines.h"

#define RD      (1)
#define WR      (0)
#define BYTE0   (0)
#define BYTE1   (1)
#define INIT    (0)
#define ACTIVE  (1)
#define set_cs(usci) \
{ \
    *usci.port_out |= usci.cs; \
}
#define clr_cs(usci) \
{ \
    *usci.port_out &= ~usci.cs; \
}

static uint8_t init_seq[][3]    = {
    {WR, I2CDIS, DISI2C},       // DISABLE I2C
    {RD, WIA1, 0x00},           // READ COMPANY ID
    {RD, WIA2, 0x00},           // READ DEVICE ID
    {WR, CNTL1, NSF},           // NOISE SUPPRESSION
    {WR, CNTL2, SDR | 0x02}     // CONTINUOUS MODE 1 (10 Hz)
};

static uint8_t active_seq[][3]  = {
    {RD, HXL, 0x00},
    {RD, HXH, 0x00},
    {RD, HYL, 0x00},
    {RD, HYH, 0x00},
    {RD, HZL, 0x00},
    {RD, HZH, 0x00}
};

typedef struct {
    volatile unsigned char *port_out;
    volatile unsigned char *port_ifg;
    uint8_t state;
    uint8_t ptr;
    uint8_t ptr_state;
    uint8_t port;
    uint8_t cs;
    ak09915_callback_t callback;
    uint16_t x;
    uint16_t y;
    uint16_t z;
} usci_t;

static usci_t usci_inst[2];

static void _setup_gpio(uint8_t port, uint8_t cs, uint8_t sck, uint8_t miso, uint8_t mosi, uint8_t interrupt) {
    volatile unsigned char *sel  = (port == 1) ? &P1SEL : &P2SEL;
    volatile unsigned char *sel2 = (port == 1) ? &P1SEL2 : &P2SEL2;
    volatile unsigned char *dir  = (port == 1) ? &P1DIR : &P2DIR;
    volatile unsigned char *ie   = (port == 1) ? &P1IE : &P2IE;
    volatile unsigned char *ies  = (port == 1) ? &P1IES : &P2IES;

    *dir        |= cs | BIT6;
    *sel        |= miso | mosi | sck;
    *sel2       |= miso | mosi | sck;
    *ie         |= interrupt;
    *ies        &= ~interrupt;
}

static void _setup_spi(uint8_t usci) {
    volatile unsigned char *ctl0 = (usci == 'A') ? &UCA0CTL0 : &UCB0CTL0;
    volatile unsigned char *ctl1 = (usci == 'A') ? &UCA0CTL1 : &UCB0CTL1;
    // uint8_t               txie   = (usci == 'A') ? UCA0TXIE : UCB0TXIE;
    uint8_t               rxie   = (usci == 'A') ? UCA0RXIE : UCB0RXIE;

    *ctl0       |= UCCKPH;              // CAPTURE ON RISING EDGE
    *ctl0       |= UCMSB;               // MSB FIRST
    *ctl0       |= UCMST;               // MASTER MODE
    *ctl0       |= UCSYNC;              // SYNC MODE
    *ctl1       |= UCSSEL_1;            // SEL ACLK
    *ctl1       &= ~UCSWRST;            // BEGIN
    IE2         |= rxie;         // ENABLE RX/TX INTERRUPTS
}

static void _exec_state(uint8_t idx) {
    volatile unsigned char *txbuf    = (idx) ? &UCB0TXBUF : &UCA0TXBUF;
    uint8_t                (*seq)[3] = (usci_inst[idx].state == INIT) ? init_seq : active_seq;

    clr_cs(usci_inst[idx]);

    if (usci_inst[idx].ptr_state == BYTE0) {
        *txbuf = (seq[usci_inst[idx].ptr][0] << 7) | seq[usci_inst[idx].ptr][1];
    } else {
        *txbuf = seq[usci_inst[idx].ptr][2];
    }
}

static void _advance_state(uint8_t idx) {
    if (usci_inst[idx].state == INIT) {
        if (usci_inst[idx].ptr_state == BYTE1) {
            set_cs(usci_inst[idx]);
        }

        usci_inst[idx].ptr        += usci_inst[idx].ptr_state;      // INC IF ON BYTE1
        usci_inst[idx].ptr_state  = !usci_inst[idx].ptr_state;      // FLIP FROM BYTE0 TO BYTE1, VICE VERSA
        if (usci_inst[idx].ptr == sizeof(init_seq)/sizeof(init_seq[0])) {
            usci_inst[idx].state  = ACTIVE;
            usci_inst[idx].ptr    = 0;
        }
    } else {
        if (!(usci_inst[idx].ptr == 0 && usci_inst[idx].ptr_state == BYTE0)) {
            usci_inst[idx].ptr++;
        }
        usci_inst[idx].ptr_state = BYTE1;
        if (usci_inst[idx].ptr == sizeof(active_seq)/sizeof(active_seq[0])) {
            usci_inst[idx].ptr_state = BYTE0;
            usci_inst[idx].ptr       = 0;
            set_cs(usci_inst[idx]);
        }
    }

    if (!(usci_inst[idx].state == ACTIVE && usci_inst[idx].ptr == 0 && usci_inst[idx].ptr_state == BYTE0)) {
        _exec_state(idx);
    }
}

static void _rx_interrupt(uint8_t idx, uint8_t rxbuf, uint8_t stat) {
    if (usci_inst[idx].ptr_state == BYTE1) {
        if (usci_inst[idx].state == ACTIVE) {
            switch (usci_inst[idx].ptr) {
                case 0: usci_inst[idx].x |= rxbuf;
                        break;
                case 1: usci_inst[idx].x |= rxbuf << 8;
                        break;
                case 2: usci_inst[idx].y |= rxbuf;
                        break;
                case 3: usci_inst[idx].y |= rxbuf << 8;
                        break;
                case 4: usci_inst[idx].z |= rxbuf;
                        break;
                case 5: usci_inst[idx].z |= rxbuf << 8;
                        usci_inst[idx].callback((int16_t)usci_inst[idx].x, (int16_t)usci_inst[idx].y, (int16_t)usci_inst[idx].z);
                        usci_inst[idx].x = usci_inst[idx].y = usci_inst[idx].z = 0;
                        break;
            }
        }
    }
    _advance_state(idx);
}

// TODO: remove interrupt, replace with called function
#if defined(AK09915_USCIA) || defined (AK09915_USCIB)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) usci_rx_vector(void) {
    #ifdef AK09915_USCIA
    if (IFG2 & UCA0RXIFG) {
        _rx_interrupt(0, UCA0RXBUF, UCA0STAT);
        IFG2 &= ~UCA0RXIFG;
    }
    #endif
    #ifdef AK09915_USCIB
    if (IFG2 & UCB0RXIFG) {
        _rx_interrupt(1, UCB0RXBUF, UCB0STAT);
        IFG2 &= ~UCB0RXIFG;
    }
    #endif
}
#endif

static void _gpio_interrupt(uint8_t port) {
    uint8_t idx;

    for (idx = 0; idx < 2; idx++) {
        if (usci_inst[idx].port == port && usci_inst[idx].state == ACTIVE && usci_inst[idx].ptr == 0 && usci_inst[idx].ptr_state == BYTE0) {
            _exec_state(idx);
        }
    }
}

#ifdef AK09915_PORT1
void __attribute__ ((interrupt(PORT1_VECTOR))) port1_vector(void) {
    _gpio_interrupt(1);
    P1IFG = 0;
}
#endif

#ifdef AK09915_PORT2
void __attribute__ ((interrupt(PORT2_VECTOR))) port2_vector(void) {
    _gpio_interrupt(2);
    P2IFG = 0;
}
#endif

void ak09915_init(uint8_t usci, uint8_t port, uint8_t cs, uint8_t sck, uint8_t miso, uint8_t mosi, uint8_t interrupt, ak09915_callback_t callback) {
    uint8_t idx = (usci == 'A') ? 0 : 1;

    _setup_gpio(port, cs, sck, miso, mosi, interrupt);
    _setup_spi(usci);

    usci_inst[idx].callback  = callback;
    usci_inst[idx].port_out  = (port == 1) ? &P1OUT : &P2OUT;
    usci_inst[idx].port_ifg  = (port == 1) ? &P1IFG : &P2IFG;
    usci_inst[idx].port      = port;
    usci_inst[idx].cs        = cs;

    set_cs(usci_inst[idx]);
    _exec_state(idx);
}
