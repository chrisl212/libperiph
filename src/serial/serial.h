#ifndef SERIAL_H
#define SERIAL_H

void serial_init(void);
void serial_tx_interrupt(void);
void serial_rx_interrupt(void);
void serial_send(const char *msg);
void serial_send_int(int num);

#endif
