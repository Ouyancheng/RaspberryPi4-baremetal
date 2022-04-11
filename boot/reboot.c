#include "boot.h"
/**
 * Set up the watchdog timer then don't feed it, 
 * and then the dog will bite and the system will reboot 
 */
noreturn void rpi_reboot(void) {
    const uintptr_t PM_RSTC = MMIO_BASE + 0x0010001c;
    const uintptr_t PM_WDOG = MMIO_BASE + 0x00100024;
    const uintptr_t PM_PASSWORD = 0x5a000000;
    const uintptr_t PM_RSTC_WRCFG_FULL_RESET = 0x00000020;
    put32(PM_WDOG, PM_PASSWORD | 1);
    put32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
    while(1); 
}
