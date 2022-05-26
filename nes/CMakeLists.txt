project(nes)
cmake_minimum_required(VERSION 3.10)
include_guard(GLOBAL)
if (NOT DEFINED UNIX)
    add_subdirectory(libpipi libpipi)
else()
    find_package(SDL2 REQUIRED)
endif()
set(TARGET nes)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(nes_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}") 
set(nes_code_files 
    cpu.c 
    cpu_opcodes.c
    main.c 
    sdk.c 
    bus.c 
    rom.c 
    cpu_debug.c 
)
if (NOT DEFINED UNIX)
    add_library(${TARGET} 
        ${nes_code_files}
    )
    target_link_libraries(${TARGET} 
        libpipi
    )
    set_target_properties(${TARGET}  PROPERTIES PREFIX "")
    set_target_properties(${TARGET}  PROPERTIES OUTPUT_NAME ${TARGET})
    set_target_properties(${TARGET}  PROPERTIES SUFFIX ".a")
else()
    set(nes_INCLUDE_DIRS ${nes_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
    add_executable(${TARGET} ${nes_code_files})
    target_link_libraries(${TARGET} ${SDL2_LIBRARIES})
endif()
target_include_directories(${TARGET} PUBLIC "${nes_INCLUDE_DIRS}")



