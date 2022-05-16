#pragma once 
#include "common.h"
#ifdef __cplusplus
extern "C" {
#endif 
unsigned timer_get_usec_raw(void);
// in usec
unsigned timer_get_usec(void);
static inline unsigned get_current_time_us(void) {
    return timer_get_usec(); 
}
void delay_us(unsigned us);
void delay_ms(unsigned ms);
void delay_sec(unsigned sec);
#ifdef __cplusplus
}
#endif 
