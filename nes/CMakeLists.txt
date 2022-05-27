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
    cpu_exec.c 
    main.c 
    sdk.c 
    bus.c 
    rom.c 
    cpu_debug.c 
    ppu.c 
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


    # set(CMAKE_C_FLAGS "-Ofast -flto -march=native")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result OUTPUT output)
    if(result)
        set_property(TARGET ${TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "IPO is not supported: ${output}")
    endif()
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_OBJDUMP} -C -D "${TARGET}"  > "${TARGET}.list"
        COMMENT "Invoking: objdump")

endif()
target_include_directories(${TARGET} PUBLIC "${nes_INCLUDE_DIRS}")



