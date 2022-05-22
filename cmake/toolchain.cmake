set(CMAKE_SYSTEM_NAME               Linux)
set(CMAKE_SYSTEM_PROCESSOR          arm64)

set(CMAKE_TRY_COMPILE_TARGET_TYPE   STATIC_LIBRARY)
set(BAREMETAL_ARM_TOOLCHAIN_PATH "/Users/ouyancheng/homebrew/opt/llvm/bin/")
set(CMAKE_EXECUTABLE_SUFFIX "")
set(CMAKE_AR                        "${BAREMETAL_ARM_TOOLCHAIN_PATH}llvm-ar${CMAKE_EXECUTABLE_SUFFIX}")
set(CMAKE_ASM_COMPILER              "${BAREMETAL_ARM_TOOLCHAIN_PATH}clang${CMAKE_EXECUTABLE_SUFFIX}")
set(CMAKE_C_COMPILER                "${BAREMETAL_ARM_TOOLCHAIN_PATH}clang${CMAKE_EXECUTABLE_SUFFIX}")
set(CMAKE_CXX_COMPILER              "${BAREMETAL_ARM_TOOLCHAIN_PATH}clang++${CMAKE_EXECUTABLE_SUFFIX}")
set(CMAKE_LINKER                    "${BAREMETAL_ARM_TOOLCHAIN_PATH}ld.lld${CMAKE_EXECUTABLE_SUFFIX}")
set(CMAKE_OBJCOPY                   "${BAREMETAL_ARM_TOOLCHAIN_PATH}llvm-objcopy${CMAKE_EXECUTABLE_SUFFIX}")
set(CMAKE_RANLIB                    "${BAREMETAL_ARM_TOOLCHAIN_PATH}llvm-ranlib${CMAKE_EXECUTABLE_SUFFIX}"  )
set(CMAKE_SIZE                      "${BAREMETAL_ARM_TOOLCHAIN_PATH}llvm-size${CMAKE_EXECUTABLE_SUFFIX}"  )
set(CMAKE_STRIP                     "${BAREMETAL_ARM_TOOLCHAIN_PATH}llvm-strip${CMAKE_EXECUTABLE_SUFFIX}"  )
set(CMAKE_OBJDUMP                   "${BAREMETAL_ARM_TOOLCHAIN_PATH}llvm-objdump${CMAKE_EXECUTABLE_SUFFIX}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
set(CPU_TYPE "cortex-a72+nosimd")
set(TARGET_TRIPLET "aarch64-unknown-none-elf") 
set(TARGET_ARCH "armv8-a")
# CLANGFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -mcpu=cortex-a72+nosimd 
# -nostartfiles  in C_FLAGS + ASM_FLAGS? 
set(C_FLAGS_COMMON                  "-Wall -nostdlib -ffreestanding -mcpu=${CPU_TYPE} --target=${TARGET_TRIPLET} -march=${TARGET_ARCH} -Werror -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable")
set(CMAKE_C_FLAGS                   "-O3 ${C_FLAGS_COMMON} -std=gnu11 -Wno-pointer-sign ")
set(CMAKE_C_FLAGS_RELEASE           "-Os -DNDEBUG ${C_FLAGS_COMMON} -std=gnu11 -Wno-pointer-sign ")
set(CMAKE_CXX_FLAGS                 "-O3 ${C_FLAGS_COMMON} -std=gnu++17 -fno-exceptions -fno-unwind-tables -fno-rtti")
set(CMAKE_CXX_FLAGS_RELEASE         "-Os -DNDEBUG ${C_FLAGS_COMMON} -std=gnu++17 -fno-exceptions -fno-unwind-tables -fno-rtti")
set(CMAKE_ASM_FLAGS                 "-nostdlib -ffreestanding --target=${TARGET_TRIPLET} -Wa,--fatal-warnings -Wa,-mcpu=${CPU_TYPE} -Wa,-march=${TARGET_ARCH} -Wno-unused-variable -Wno-unused-but-set-variable")


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


