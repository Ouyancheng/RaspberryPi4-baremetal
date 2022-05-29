#pragma once 
#define DMA_USE_INTERRUPT 0  
#define DEBUG_DMA 0 
#if DMA_USE_INTERRUPT && defined(BCM2835)
#include "interrupts.h"
#endif 
#include "common.h"
#include "printf.h"
#include "mbox.h"
struct DMAControlBlock {
    volatile uint32_t transfer_info; 
    volatile uint32_t src_addr; 
    volatile uint32_t dst_addr;
    volatile uint32_t transfer_len;
    volatile uint32_t mode_stride; 
    volatile uint32_t next_cb_addr;
    volatile uint32_t reserved1;
    volatile uint32_t reserved2; 
} __attribute__ ((aligned(32)));
struct DMAInfo {
    uint32_t channel;
    volatile uint32_t *reg_file; 
    volatile struct DMAControlBlock *cb;
    volatile uint32_t dma_done; 
};


#define DMA_CH0_REGS ((uintptr_t)0x00007000 + MMIO_BASE)
#define DMA_CH_REG_OFFSET 0x100 
#define DMA_CS_IDX (0x0>>2)
#define DMA_CBLK_ADDR_IDX (0x4>>2) 
#define DMA_TI_IDX (0x8>>2) 
#define DMA_SRC_ADDR_IDX (0xC>>2) 
#define DMA_DST_ADDR_IDX (0x10>>2) 
#define DMA_TXFR_LEN_IDX (0x14>>2) 
#define DMA_STRIDE_IDX (0x18>>2) 
#define DMA_NEXT_CB_IDX (0x1C>>2) 
#define DMA_DEBUG_IDX (0x20>>2) 

#define DMA_CS_INT (1 << 2)
#define DMA_CS_ACTIVE (1 << 0)
#define DMA_CS_END (1 << 1)
#define DMA_CS_ERR (1 << 8) 
#define DMA_CS_RESET (1 << 31) 
#define DMA_CS_DISDEBUG (1 << 29) 
#define DMA_REG_ENABLE (DMA_CH0_REGS + 0xFF0)
#define DMA_REG_INT_STATUS (DMA_CH0_REGS + 0xFE0) 

#define IRQ_DMA0 16 
#define DMA_NUM_CHANNELS 15 
#define DMA_DEV2MEM 0 
#define DMA_MEM2DEV 1 
#define DMA_MEM2MEM 2 
#define DMA_DEV2DEV 3 
#define BUS2ARM_MEM_ADDR(addr) ((uint32_t)(addr) & (~0xC0000000))





#ifdef BCM2835
// why 0x40000000, see: https://forums.raspberrypi.com/viewtopic.php?t=280262 
#define RPI1_DMA_ARM2BUS_MEM_ADDR(addr) (((uint32_t)(addr) & ~0xC0000000) | 0x40000000)
// NOTE: the above code only works when GPU L2 cache is enabled (default), 
// and we can also disable the GPU L2 cache in config.txt 
// if the GPU L2 cache is disabled, we should use the 0xC alias 
#define DMA_ARM2BUS_MEM_ADDR(addr) RPI1_DMA_ARM2BUS_MEM_ADDR(addr) 
static inline uint32_t bcm2835_dma_get_irq(uint32_t ch) {
    if (ch < 11) {
        return (IRQ_DMA0 + ch);
    } else {
        return 27;  // 11 - 14 will use IRQ 27 
    }
}
#else  

#define DMA_ARM2BUS_MEM_ADDR(addr) (((uintptr_t)(addr) & ~0xC0000000) | 0xC0000000)
#define ARM2BUS_PERI_ADDR(addr) ((uintptr_t)(addr) - MMIO_BASE + BUS_MMIO_BASE)

#endif 

void dma_interrupt_handler(unsigned pc, void *dma_info);

#define DMA_SRC_DREQ (1 << 10)
#define DMA_SRC_INC (1 << 8)
#define DMA_DST_DREQ (1 << 6)
#define DMA_DST_INC (1 << 4)
#define DMA_TI_INT_EN (1 << 0)

void dma_init(); 

int dma_start(
    uint32_t channel, 
    uint32_t device, 
    uint32_t direction, 
    void *src, 
    void *dest, 
    uint32_t len
);

int dma_wait(uint32_t channel);


extern volatile struct DMAInfo channels[DMA_NUM_CHANNELS];  // 0 - 14... 
extern volatile struct DMAControlBlock cbs[DMA_NUM_CHANNELS] __attribute__((aligned(32))); 






