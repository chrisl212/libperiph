#ifndef CFAP400300_H
#define CFAP400300_H

void cfap400300_init(void);

#define PSR             (0x00)          // PANEL SETTING REGISTER
#define RES1            (0x80)
#define RES0            (0x40)
#define REG_EN          (0x20)
#define BWR             (0x10)
#define UD              (0x08)
#define SHL             (0x04)
#define SHD_N           (0x02)
#define RST_N           (0x01)

#define PWR             (0x01)          // POWER SETTING REGISTER
#define VDS_EN          (0x02)
#define VDG_EN          (0x01)
#define VCOM_HV         (0x04)
#define VGHL_LV         (0x03)
#define VDH             (0x3F)
#define VDL             (0x3F)
#define VDHR            (0x3F)

#define PWR_OFF         (0x02)          // POWER OFF REGISTER

#define PFS             (0x03)          // POWER OFF SEQUENCE SETTING
#define T_VDS_OFF       (0x30)

#define PON             (0x04)          // POWER ON REGISTER

#define PMES            (0x05)          // POWER ON MEASSURE REGISTER

#define BTST            (0x06)          // BOOSTER SOFT START REGISTER
#define BTPHA7          (0x80)
#define BTPHA6          (0x40)
#define BTPHA5          (0x20)
#define BTPHA4          (0x10)
#define BTPHA3          (0x08)
#define BTPHA2          (0x04)
#define BTPHA1          (0x02)
#define BTPHA0          (0x01)
#define BTPHB7          (0x80)
#define BTPHB6          (0x40)
#define BTPHB5          (0x20)
#define BTPHB4          (0x10)
#define BTPHB3          (0x08)
#define BTPHB2          (0x04)
#define BTPHB1          (0x02)
#define BTPHB0          (0x01)
#define BTPHC5          (0x20)
#define BTPHC4          (0x10)
#define BTPHC3          (0x08)
#define BTPHC2          (0x04)
#define BTPHC1          (0x02)
#define BTPHC0          (0x01)

#define DSLP            (0x07)          // DEEP SLEEP REGISTER
#define DSLP_CHK        (0xA5)

#define DST             (0x10)          // DATA START TRANSMISSION

#define DSP             (0x11)          // DATA STOP TRANSMISSION

#define DSR             (0x12)          // DISPLAY REFRESH

#define IPC             (0x13)          // IMAGE PROCESS COMMAND
#define IP_EN           (0x10)
#define IP_SEL          (0x07)

#define LUTC            (0x20)          // BUILD VCOM LUT COMMAND

#define LUTWW           (0x21)          // BUILD WHITE-TO-WHITE LUT COMMAND

#define LUTBW           (0x22)          // BUILD BLACK-TO-WHITE LUT COMMAND

#define LUTWB           (0x23)          // BUILD WHITE-TO-BLACK LUT COMMAND

#define LUTBB           (0x24)          // BUILD BLACK-TO-BLACK LUT COMMAND

#define PLL             (0x30)          // PLL CONTROL COMMAND
typedef enum {
    PLL_29HZ    = 0x09,
    PLL_14HZ,
    PLL_10HZ,
    PLL_7HZ,
    PLL_6HZ,
    PLL_5HZ,
    PLL_4HZ,
    PLL_57HZ    = 0x11,
    PLL_29HZ0,
    PLL_19HZ,
    PLL_14HZ0,
    PLL_11HZ,
    PLL_10HZ0,
    PLL_8HZ,
    PLL_86HZ    = 0x19,
    PLL_43HZ,
    PLL_29HZ1,
    PLL_21HZ,
    PLL_17HZ,
    PLL_14HZ1,
    PLL_12HZ,
    PLL_114HZ   = 0x21,
    PLL_57HZ0,
    PLL_38HZ,
    PLL_29HZ2,
    PLL_23HZ,
    PLL_19HZ0,
    PLL_16HZ,
    PLL_150HZ   = 0x29,
    PLL_72HZ,
    PLL_48HZ,
    PLL_36HZ,
    PLL_29HZ3,
    PLL_24HZ,
    PLL_20HZ,
    PLL_171HZ   = 0x31,
    PLL_86HZ0,
    PLL_57HZ1,
    PLL_43HZ0,
    PLL_34HZ,
    PLL_29HZ4,
    PLL_24HZ0,
    PLL_200HZ   = 0x39,
    PLL_100HZ,
    PLL_67HZ,
    PLL_50HZ,
    PLL_40HZ,
    PLL_33HZ,
    PLL_29HZ5
} pll_freq_e;

#define TSC             (0x40)          // TEMPERATURE SENSOR CALIBRATION COMMAND

#define TSE             (0x41)          // TEMPERATURE SENSOR ENABLE COMMAND
#define TSE_DISABLE     (0x80)

#define TSW             (0x42)          // TEMPERATURE SENSOR WRITE COMMAND

#define TSR             (0x43)          // TEMPERATURE SENSOR READ COMMAND

#define CDI             (0x50)          // VCOM AND DATA INTERVAL SETTING COMMAND

#define LPD             (0x51)          // LOW POWER DETECTION COMMAND

#define TCON            (0x60)          // TCON SETTING COMMAND

#define TRES            (0x61)          // RESOLUTION SETTING COMMAND

#define GSST            (0x65)          // GSST SETTING COMMAND

#define FLG             (0x71)          // GET FLAG STATUS COMMAND

#define AMV             (0x80)          // AUTO MEASURE VCOM COMMAND

#define VV              (0x81)          // VCOM VALUE COMMAND

#define VDCS            (0x82)          // VCOM-DC SETTING COMMAND

#define PTL             (0x90)          // PARTIAL WINDOW COMMAND

#define PTIN            (0x91)          // PARTIAL IN COMMAND

#define PTOUT           (0x92)          // PARTIAL OUT COMMAND

#define PGM             (0xA0)          // PROGRAM MODE COMMAND
#define PGM_DATA        (0xA5)

#define APG             (0xA1)          // ACTIVE PROGRAM MODE COMMAND

#define ROTP            (0xA2)          // READ OTP DATA COMMAND

#define PWS             (0xE3)          // POWER SAVING COMMAND

#endif
