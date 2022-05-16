#pragma once 
#include <stdint.h>
#include <stddef.h>
#include <stdnoreturn.h>
#ifdef __cplusplus
extern "C" {
#endif 
static inline void dev_barrier() {
    asm volatile("dsb sy");
}
int main(void); 
noreturn void rpi_reboot(void); 
static inline void put32(uintptr_t addr, uint32_t v) {
    *((volatile uint32_t *)addr) = v; 
}
static inline uint32_t get32(uintptr_t addr) {
    return *((volatile uint32_t *)addr); 
}

#ifdef __cplusplus
}
#endif 
#define MMIO_BASE (0xFE000000)
