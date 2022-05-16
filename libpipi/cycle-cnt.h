#pragma once 

#ifdef __cplusplus
extern "C" {
#endif 

// must do init first.
#define cycle_cnt_init() do {                                   \
    unsigned in = 1;                                            \
    asm volatile("MCR p15, 0, %0, c15, c12, 0" :: "r"(in));     \
} while(0)

// read.  should add a write().
#define cycle_cnt_read() ({                                     \
    unsigned _out;                                              \
    asm volatile ("MRC p15, 0, %0, c15, c12, 1" : "=r"(_out));  \
    _out;                                                       \
})

inline __attribute__((always_inline)) unsigned delay_ncycles(unsigned start, unsigned n) {
    // this handles wrap-around, but doing so adds two instructions,
    // which makes the delay not as tight.
    unsigned c;
    while(((c = cycle_cnt_read()) - start) < n);
    return c;
}

#ifdef __cplusplus
}
#endif 

