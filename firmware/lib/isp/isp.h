#ifndef _ISP_H_
#define _ISP_H_

#include <Arduino.h>
void isp_init();
void start_pmode();
void end_pmode();
uint32_t isp_read_signature();
void program_to_flash(uint8_t* buff, int length);
void program_fuses(uint8_t high_fuse, uint8_t low_fuse);

#endif
