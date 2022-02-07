#include <msp430g2553.h>
#include <stdint.h>
#include "serial.h"

#define A0RX    (BIT1)
#define A0TX    (BIT2)

struct {
    uint8_t     active   ; //: 1;
    uint8_t     msg_done ; //: 1;
    uint16_t    msg_ptr  ; //: 14;
    const char  *msg;
} serial_port;

static void _setup_gpio(void) {
    P1SEL       |= A0RX | A0TX;
    P1SEL2      |= A0RX | A0TX;
}

static void _setup_uart(void) {
    // for 115200 with freq of 4.25 MHz
    // BR = 36 (>= 16, can oversample)
    // BR = INT(36/16) = 2
    // BRF = INT(((BR/16)-INT(BR/16)) * 16) = 4
    
    UCA0CTL1    = UCSWRST;          // START
    // UCA0CTL0    = UCPEN;          // START
    // UCA0CTL0    = UCSPB;          // START
    UCA0BR0     = 78;
    UCA0BR1     = 0;
    UCA0MCTL    = (2 << 4)| UCOS16;
    UCA0CTL1    |= UCSSEL_2;          // SMCLK
    UCA0CTL1    &= ~UCSWRST;          // START

    IE2         |= UCA0TXIE;
}

void serial_init(void) {
    _setup_gpio();
    _setup_uart();
    serial_port.active = 1;
    serial_port.msg_done = 1;
}

static void _tx_char(void) {
    while (UCA0STAT & UCBUSY);

    if (serial_port.msg && serial_port.msg[serial_port.msg_ptr]) {
        UCA0TXBUF = serial_port.msg[serial_port.msg_ptr++];
        serial_port.msg_done = !serial_port.msg[serial_port.msg_ptr];
    }
}

void serial_tx_interrupt(void) {
    if (serial_port.active && !serial_port.msg_done && (IFG2 & UCA0TXIFG)) {
        _tx_char();
    }
    IFG2 &= ~UCA0TXIFG;
}

void serial_send(const char *msg) {
    while (!serial_port.msg_done);
    serial_port.msg      = msg;
    serial_port.msg_done = 0;
    serial_port.msg_ptr  = 0;
    _tx_char();
}

void serial_send_int(int num) {
    static char str[10];
    int         denom = 1;
    int         dig = 0;

    while (!serial_port.msg_done);

    while (num/denom >= 10) denom *= 10;
    while (denom) {
        str[dig++] = '0' + (num / denom);
        num %= denom;
        denom /= 10;
    }
    str[dig] = 0;

    serial_send(str);
}
