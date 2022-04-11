project(bootloader)
cmake_minimum_required(VERSION 3.9)
set(TARGET bootloader)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(../cmake/common.cmake)
# add_subdirectory(${PROJECT_ROOT} "cmake-build-libpi")
# message(STATUS "CMAKELINKER = ${CMAKE_LINKER}")

# set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_LINKER} -T ${LINKER_SCRIPT}")
add_executable(${TARGET}.elf 
    kernel.cpp 
    boot.S 
    gpio.c
    delay.c
    mbox.c 
    uart.cpp 
)

# target_include_directories(${TARGET}.elf PRIVATE "${LIBPI_ROOT}/include" "${LIBPI_ROOT}/libc" "${LIBPI_ROOT}/")
# target_link_libraries(${TARGET}.elf PUBLIC libpi)

# set_target_properties(${TARGET}.elf PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})


add_custom_command(TARGET ${TARGET}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ${TARGET}.elf -O binary ${TARGET}.bin
    COMMENT "Invoking: objcopy")

add_custom_command(TARGET ${TARGET}.elf POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -C -D ${TARGET}.elf  > ${TARGET}.list
    COMMENT "Invoking: objdump")


# add_custom_command(TARGET ${TARGET}.elf POST_BUILD
#     COMMAND pi-install ${TARGET}.bin
#     COMMENT "Invoking: Running")


