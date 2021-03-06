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
#ifdef BCM2835
#define MMIO_BASE (UINT32_C(0x20000000))
#define BUS_MMIO_BASE (UINT32_C(0x7E000000))
#else 
#define MMIO_BASE ((uintptr_t)(0xFE000000))
#define BUS_MMIO_BASE ((uintptr_t)(0x7E000000))
#endif 


