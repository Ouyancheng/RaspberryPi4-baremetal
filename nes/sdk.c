#include "sdk.h"
void do_nothing(void) {
    return; 
}

int panic_at(const char *file, int line, const char *function, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int n = 0; 
    n += printf("At file: %s:%d function %s...\nPANIC: ", file, line, function); 
    n += vprintf(format, args); 
    va_end(args); 
    exit(1); 
    return n; 
}
#ifdef PLATFORM_UNIX 
unsigned load_file(uint8_t *target, unsigned target_size, const char *filename) {
    FILE *romfile = fopen(filename, "rb"); 
    // FILE *romfile = fopen("../testroms/snake.nes", "rb"); 
    // FILE *romfile = fopen("../testroms/nestest.nes", "rb"); 
    // FILE *romfile = fopen("../testroms/smb1.nes", "rb"); 
    // FILE *romfile = fopen("../testroms/DonkeyKong.nes", "rb"); 
    // FILE *romfile = fopen("../testroms/Pac-Man (USA) (Namco).nes", "rb"); 
    // FILE *romfile = fopen("../testroms/tetrisa.nes", "rb"); 
    // FILE *romfile = fopen("../testroms/Alter_Ego.nes", "rb"); 
    if (!romfile) {
        panic("romfile not found!\n"); 
    }
    size_t romsize; 
    fseek(romfile, 0L, SEEK_END);
    romsize = ftell(romfile);
    if (romsize > target_size) {
        fclose(romfile); 
        panic("rom file size is %zu, but target buffer size is %u\n", romsize, target_size); 
    }
    rewind(romfile); 
    if (fread((uint8_t*)target, romsize, 1, romfile) != 1) {
        panic("rom file read failed!\n"); 
    }
    fclose(romfile); 
    return romsize; 
}
#endif 


