project(secondary)
cmake_minimum_required(VERSION 3.10)
set(TARGET secondary)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/link.ld) 
include(../cmake/common.cmake)
add_executable(${TARGET}.elf 
    main.c 
    start.S 
)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../libpipi" libpipi)
# message(STATUS "libpipi_INCLUDE_DIRS=${libpipi_INCLUDE_DIRS}")
# target_include_directories(${TARGET}.elf PUBLIC libpipi_INCLUDE_DIRS)
target_link_libraries(${TARGET}.elf PUBLIC libpipi) 
add_custom_command(TARGET ${TARGET}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.elf" -O binary "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.bin"
    COMMENT "Invoking: objcopy")

add_custom_command(TARGET ${TARGET}.elf POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -C -D "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.elf"  > "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.list"
    COMMENT "Invoking: objdump")


# add_custom_command(TARGET ${TARGET}.elf POST_BUILD
#     COMMAND pi-install ${TARGET}.bin
#     COMMENT "Invoking: Running")


