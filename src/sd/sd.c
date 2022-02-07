#include <stdio.h>
#include <msp430g2553.h>
#include <stdint.h>
#include "sd.h"

#define SD_MOSI (BIT7)
#define SD_MISO (BIT6)
#define SD_CS   (BIT3)
#define SD_SCK  (BIT5)

#define SET_CS (P2OUT |= SD_CS)
#define CLR_CS (P2OUT &= ~SD_CS)

#define TIMEOUT   (250)
#define SECTOR_SZ (512)

typedef enum {
    SD_OKAY,
    SD_UNRECOGNIZED,
    SD_UNKNOWN_VERSION,
    SD_TIMEOUT,
    SD_READ_ERR
} sd_status_e;

typedef enum {
    SD_BYTE,
    SD_BLOCK
} sd_addressing_mode_e;

sd_addressing_mode_e addressing_mode;
uint32_t             rd_ptr;

extern void     delay(long cyc);
static uint8_t  _sd_spi_tx_byte(uint8_t dat);
static void     _sd_spi_tx_word(uint32_t dat);
static uint32_t _sd_spi_rx_word(void);
static void     _sd_setup_pins(void);
static void     _sd_setup_spi(void);
static sd_status_e     _sd_power_on_sequence(void);
static uint8_t  _sd_send_command(uint8_t idx, uint32_t arg, uint8_t a, uint32_t *ocr);
static void     _pulse(void);

void sd_init(void) {
    sd_status_e res;

    _sd_setup_pins();
    _sd_setup_spi();
    res = _sd_power_on_sequence();

    res = res;
}

static void _sd_setup_pins(void) {
    P2DIR       |= SD_CS;
    P1SEL       |= SD_SCK + SD_MOSI + SD_MISO;
    P1SEL2      |= SD_SCK + SD_MOSI + SD_MISO;

    P2OUT       |= SD_CS;
}

static void _sd_setup_spi(void) {
    UCB0CTL0    |= UCCKPH;      // DATA CAPTURED ON RISING EDGE
    UCB0CTL0    |= UCMSB;       // MSB FIRST
    UCB0CTL0    |= UCMST;       // MASTER MODE
    UCB0CTL0    |= UCMODE_0;    // 3-PIN SPI
    UCB0CTL0    |= UCSYNC;      // SYNCHRONOUS MODE
    UCB0CTL1    |= UCSSEL_2;    // SELECT SMCLK
    UCB0CTL1    &= ~UCSWRST;    // TURN ON
}

static uint8_t _sd_spi_tx_byte(uint8_t dat) {
    UCB0TXBUF = dat;
    while (!(IFG2 & UCB0RXIFG));//(UCB0STAT & UCBUSY);

    return UCB0RXBUF;
}

static void _sd_spi_tx_word(uint32_t word) {
    int8_t  shift_amount;
    uint8_t len = 8*sizeof(word);

    for (shift_amount = len-8; shift_amount >= 0; shift_amount -= 8) {
        uint8_t a = (word >> shift_amount) & 0xff;
        _sd_spi_tx_byte(a);
    }
}

static uint32_t _sd_spi_rx_word(void) {
    uint32_t resp = 0;
    uint8_t  len = sizeof(resp);
    uint8_t  byte;

    for (byte = 0; byte < len; byte++) {
        resp |= _sd_spi_tx_byte(0xff) << (8*(len - 1 - byte));
    }
    return resp;
}

static uint8_t _sd_send_command(uint8_t idx, uint32_t arg, uint8_t a, uint32_t *ocr) {
    uint8_t crc;
    uint8_t resp = 0xFF;

    if (a) {
        _sd_send_command(55, 0, 0, ocr);
    }

    _sd_spi_tx_byte(0xFF);
    CLR_CS;
    _sd_spi_tx_byte(0xFF);
    _sd_spi_tx_byte(0xFF);
    _sd_spi_tx_byte(0xFF);

    _sd_spi_tx_byte(idx | 0x40); //send index, prefixed with '01'
    _sd_spi_tx_word(arg); //send argument
    switch (idx) {
        case 0:
            crc = 0x95;
            break;
        case 1:
            crc = 0xF9;
            break;
        case 8:
            crc = 0x87;
            break;
        case 41:
            crc = 0x77;
            break;
        case 55:
            crc = 0x65;
            break;
    }
    _sd_spi_tx_byte(crc | 0x01);

    while (resp == 0xFF && (idx != 55)) {
        resp = _sd_spi_tx_byte(0xFF); //read until get a proper response
    }
    if (idx == 8 || idx == 58) {
        *ocr = _sd_spi_rx_word();
    }
    
    _sd_spi_tx_byte(0xFF);
    SET_CS;
    _sd_spi_tx_byte(0xFF);
    return resp;
}

static void _pulse(void) {
    int byte;

    //send at least 74 (80) clock pulses following power on
    //NSS remains high
    for (byte = 0; byte < 10; byte++) {
        _sd_spi_tx_byte(0xFF);
    }
}

static sd_status_e _sd_power_on_sequence(void) {
    uint8_t res;
    uint8_t timeout = 0;
    uint32_t ocr;

    addressing_mode = SD_BYTE;

    _pulse();

    res = _sd_send_command(0, 0, 0, NULL);
    if (res != 0x01) {
        return SD_UNRECOGNIZED;
    }

    res = _sd_send_command(8, 0x01AA, 0, &ocr);
    if (res != 0x01 || ocr != 0x01AA) {
        return SD_UNRECOGNIZED;
    }
    
    res = _sd_send_command(41, 0x40000000, 1, NULL);
    if (res != 0x00) {
        if (res == 0x01) {
            do {
                res = _sd_send_command(41, 0x40000000, 1, NULL);
                timeout++;
                delay(1000);
            } while (res != 0x00 && timeout < TIMEOUT);
            if (timeout == TIMEOUT) {
                return (res == 0x1) ? SD_TIMEOUT : SD_UNRECOGNIZED;
            }
        } else if (res == 0x05) {
            do {
                res = _sd_send_command(1, 0, 0, NULL);
                timeout++;
                delay(1000);
            } while (res != 0x00 && timeout < TIMEOUT);
            if (timeout == TIMEOUT) {
                return SD_TIMEOUT;
            }
        } else {
            return SD_UNKNOWN_VERSION;
        }
    }
   
    res = _sd_send_command(58, 0, 0, &ocr);
    if (ocr & 0x40000000) {
        addressing_mode = SD_BLOCK;
    }

    res = _sd_send_command(16, 0x200, 0, NULL);
    if (res != 0x00) {
        return 254;
    }

    return SD_OKAY;
}

void sd_sector_start_read(uint32_t byte_addr) {
    // uint8_t byte_idx;
    uint8_t byte, resp;

    if (addressing_mode == SD_BYTE) {
        byte_addr *= 512;
    }

    _sd_spi_tx_byte(0xFF);
    CLR_CS;
    _sd_spi_tx_byte(0xFF);

    _sd_spi_tx_byte(17 | 0x40); //send CMD17, prefixed with '01'
    _sd_spi_tx_word(byte_addr); //send addr
    _sd_spi_tx_byte(0x00); //send CRC (N/A)

    resp = _sd_spi_tx_byte(0xFF);
    while (resp == 0xFF) {
        resp = _sd_spi_tx_byte(0xFF);
    }
    if (resp == 0x00) {
        //2 extra bytes for CRC
        byte = _sd_spi_tx_byte(0xFF);
        while (byte == 0xFF) {
            byte = _sd_spi_tx_byte(0xFF);
        }
    }

    // SET_CS;
    // _sd_spi_tx_byte(0xFF);
}

uint8_t sd_sector_read(void) {
    return _sd_spi_tx_byte(0xFF);
}

void sd_sector_finish_read(void) {
    uint8_t byte_idx;

    for (byte_idx = 0; byte_idx < 2; byte_idx++) {
        // receive last two CRC bytes
        _sd_spi_tx_byte(0xFF);
    }

    SET_CS;
    _sd_spi_tx_byte(0xFF);
}
