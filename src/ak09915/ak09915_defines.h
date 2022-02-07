#ifndef AK09915_DEFINES_H
#define AK09915_DEFINES_H

#define WIA1    (0x00)  // RO COMPANY ID
#define WIA2    (0x01)  // RO DEVICE ID
#define RSV     (0x02)  // RO RESERVED
#define INFO    (0x03)  // RO INFO
#define ST1     (0x10)  // RO STATUS1
#define HXL     (0x11)  // RO MAG MEASUREMENT XLO
#define HXH     (0x12)  // RO MAG MEASUREMENT XHI
#define HYL     (0x13)  // RO MAG MEASUREMENT YLO
#define HYH     (0x14)  // RO MAG MEASUREMENT YHI
#define HZL     (0x15)  // RO MAG MEASUREMENT ZLO
#define HZH     (0x16)  // RO MAG MEASUREMENT ZHI
#define TMPS    (0x17)  // RO DUMMY
#define ST2     (0x18)  // RO STATUS2
#define CNTL1   (0x30)  // RW CONTROL1
#define CNTL2   (0x31)  // RW CONTROL2
#define CNTL3   (0x32)  // RW CONTROL3
#define TS1     (0x33)  // RW TEST (DO NOT ACCESS)
#define TS2     (0x34)  // RW TEST (DO NOT ACCESS)
#define TS3     (0x35)  // RW TEST (DO NOT ACCESS)
#define I2CDIS  (0x36)  // RW I2C DISABLE
#define TS4     (0x37)  // RW TEST (DO NOT ACCESS)
#define ASAX    (0x60)  // RO DUMMY
#define ASAY    (0x61)  // RO DUMMY
#define ASAZ    (0x62)  // RO DUMMY
#define TPH1    (0xC0)  // RW TEST (DO NOT ACCESS)
#define TPH2    (0xC1)  // RW TEST (DO NOT ACCESS)
#define RR      (0xC2)  // RW TEST (DO NOT ACCESS)
#define SYT     (0xC3)  // RW TEST (DO NOT ACCESS)
#define DT      (0xC4)  // RW TEST (DO NOT ACCESS)

// WIA
#define CID     (0x48)  // COMPANY ID
#define DID     (0x10)  // DEVICE ID

// ST1
#define DRDY    (0x01)  // DATA READY
#define DOR     (0x02)  // DATA OVERRUN
#define HSM     (0x80)  // I2C HIGH SPEED MODE

// ST2
#define INV     (0x04)  // INVALID DATA
#define HOFL    (0x08)  // MAGNETIC SENSOR OVERFLOW

// CNTL1
#define NSF     (0x20)  // NOISE SUPPRESSION

// CNTL2
#define SDR     (0x40)  // SENSOR DRIVE
#define FIFO    (0x80)  // FIFO ENABLE

// CNTL3
#define SRST    (0x01)  // SOFT RESET

// I2CDIS
#define DISI2C  (0x1B)  // DISABLE I2C PASSWORD

#endif
