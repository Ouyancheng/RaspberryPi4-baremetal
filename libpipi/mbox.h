/**
 * I am too lazy to dig into bcm2711's documentation (it should be largely similar to bcm2835 though), 
 * so I just transcribe the code from the following amazing repository: 
 * Reference: https://github.com/isometimes/rpi4-osdev/blob/master/part5-framebuffer/mb.h 
 */
#pragma once 
#ifdef __cplusplus
extern "C" {
#endif 
// a properly aligned buffer
extern volatile unsigned int mbox[36];

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
int mbox_call(unsigned char ch);

#ifdef __cplusplus
}
#endif 
