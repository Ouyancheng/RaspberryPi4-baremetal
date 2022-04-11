#pragma once 
#include "boot.h"
#ifdef __cplusplus
extern "C" {
#endif 
unsigned timer_get_usec_raw(void);
// in usec
unsigned timer_get_usec(void);

void delay_us(unsigned us);
void delay_ms(unsigned ms);
void delay_sec(unsigned sec);
#ifdef __cplusplus
}
#endif 
