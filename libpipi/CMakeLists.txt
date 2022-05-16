project(libpipi)
cmake_minimum_required(VERSION 3.10)
include_guard(GLOBAL)
set(TARGET libpipi)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(libpipi_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}") 
add_library(${TARGET} 
    gpio.c
    delay.c
    mbox.c 
    uart.c 
    reboot.c
    c_string.c 
    printf.c 
    crc.c 
    uartgetcode.c 
    framebuffer.c 
)
target_include_directories(${TARGET} PUBLIC "${libpipi_INCLUDE_DIRS}")
set_target_properties(${TARGET}  PROPERTIES PREFIX "")
set_target_properties(${TARGET}  PROPERTIES OUTPUT_NAME ${TARGET})
set_target_properties(${TARGET}  PROPERTIES SUFFIX ".a")


