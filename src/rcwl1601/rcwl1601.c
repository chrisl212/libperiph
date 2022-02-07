#include <msp430g2553.h>
#include "rcwl1601.h"

struct {
    rcwl1601_callback_t         callback;
    uint32_t                    sum;
    uint8_t                     smps;
    volatile unsigned int       *tiv;
    volatile unsigned int       *ccr2;
} sensors[2];

static void _setup_gpio(uint8_t port, uint8_t trig, uint8_t echo) {
    volatile unsigned char *sel = (port == 1) ? &P1SEL : &P2SEL;
    volatile unsigned char *dir = (port == 1) ? &P1DIR : &P2DIR;
    *sel        |= trig | echo;
    *dir        |= trig;                                // OUTPUT DIRECTION
}

static void _setup_timer(uint8_t timer) {
    volatile unsigned int *ccr0  = (timer == 1) ? &TA1CCR0 : &TA0CCR0;
    volatile unsigned int *ccr1  = (timer == 1) ? &TA1CCR1 : &TA0CCR1;
    volatile unsigned int *cctl1 = (timer == 1) ? &TA1CCTL1 : &TA0CCTL1;
    volatile unsigned int *cctl2 = (timer == 1) ? &TA1CCTL2 : &TA0CCTL2;
    volatile unsigned int *ctl   = (timer == 1) ? &TA1CTL : &TA0CTL;

    *ccr0       = 1000;                                 // PULSE FOR 1 COUNT
    *ccr1       = 0;                                    // RESET AT 0
    *cctl1      |= OUTMOD_3;                            // SET/RESET
    *cctl2      |= CM_2;                                // CAPTURE ON RISING EDGE
    *cctl2      |= CCIS_0;                              // CCI0A INPUT
    *cctl2      |= SCS;                                 // SYNC INPUT
    *cctl2      |= CAP;                                 // CAPTURE MODE
    *cctl2      |= CCIE;                                // ENABLE INTERRUPT ON COMPARE
    *ctl        |= TASSEL_2;                            // SELECT SMCLK
    *ctl        |= ID_3;                                // ACLK /= 8
    *ctl        |= MC_2;                                // CONT MODE
}

void rcwl1601_timer_interrupt(void) {
    uint8_t  timer;

    for (timer = 0; timer < 2; timer++) {
        if (!(*(sensors[timer].tiv) & TA0IV_TACCR2)) {
            continue;
        }

        sensors[timer].sum += *(sensors[timer].ccr2);
        if (++sensors[timer].smps == RCWL1601_SAMPLES) {
            sensors[timer].callback(sensors[timer].sum >> RCWL1601_SAMPLES_BITS);
            sensors[timer].sum = sensors[timer].smps = 0;
        }

    }
}

void rcwl1601_init(uint8_t timer, uint8_t port, uint8_t trig, uint8_t echo, rcwl1601_callback_t callback) {
    _setup_gpio(port, trig, echo);
    _setup_timer(timer);

    sensors[timer].callback = callback;
    sensors[timer].tiv      = (timer == 1) ? &TA1IV : &TA0IV;
    sensors[timer].ccr2     = (timer == 1) ? &TA1CCR2 : &TA0CCR2;
}
