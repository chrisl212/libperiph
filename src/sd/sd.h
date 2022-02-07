#ifndef SD_H
#define SD_H

#include <stdint.h>

void    sd_init(void);
void    sd_sector_start_read(uint32_t byte_addr);
uint8_t sd_sector_read(void);
void    sd_sector_finish_read(void);

#endif
