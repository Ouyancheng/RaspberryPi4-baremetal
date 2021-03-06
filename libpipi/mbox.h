/**
 * I am too lazy to dig into bcm2711's documentation (it should be largely similar to bcm2835 though), 
 * so I just transcribe the code from the following amazing repository: 
 * Reference: https://github.com/isometimes/rpi4-osdev/blob/master/part5-framebuffer/mb.h 
 */
#pragma once 
#ifdef __cplusplus
extern "C" {
#endif 
// a properly aligned buffer (do we need to specify the attribute here, or just the definition is enough)
extern volatile unsigned int __attribute__((aligned(16))) mbox[36];

#define MBOX_REQUEST    0

// channels
#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8

// tags
#define MBOX_TAG_GETSERIAL      0x10004
#define MBOX_TAG_SETCLKRATE     0x38002
#define MBOX_TAG_SETPHYWH       0x48003 
#define MBOX_TAG_SETVIRTWH      0x48004 
#define MBOX_TAG_SETVIRTOFF     0x48009 
#define MBOX_TAG_SETDEPTH       0x48005 
#define MBOX_TAG_SETPXLORDR     0x48006 
#define MBOX_TAG_GETFB          0x40001 
#define MBOX_TAG_GETPITCH       0x40008 
#define MBOX_TAG_LAST           0x00000
int mbox_call(unsigned char ch);
#define VIDEOCORE_MBOX  (MMIO_BASE+0x0000B880)
#define MBOX_READ       ((volatile unsigned int*)(VIDEOCORE_MBOX+0x0))
#define MBOX_POLL       ((volatile unsigned int*)(VIDEOCORE_MBOX+0x10))
#define MBOX_SENDER     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x14))
#define MBOX_STATUS     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x18))
#define MBOX_CONFIG     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x1C))
#define MBOX_WRITE      ((volatile unsigned int*)(VIDEOCORE_MBOX+0x20))
#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000
#ifdef __cplusplus
}
#endif 
