#include <msp430g2553.h>
#include "misc.h"

void delay(long int cyc) {
    while (cyc--);
}
