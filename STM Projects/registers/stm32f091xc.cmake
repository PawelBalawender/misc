SET(CMAKE_SYSTEM_NAME Windows)
SET(CMAKE_SYSTEM_PROCESSOR ARM)
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(PROJECT_SOURCE_DIR C:/Users/Pawel/Desktop/kisaburo)
SET(CMAKE_C_COMPILER "C:/Users/Pawel/Desktop/gcc-arm-none-eabi-7-2018-q2-update-win32/bin/arm-none-eabi-gcc.exe")
SET(CMAKE_CXX_COMPILER "C:/Users/Pawel/Desktop/gcc-arm-none-eabi-7-2018-q2-update-win32/bin/arm-none-eabi-g++.exe")

SET(LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/include/STM32F091RCTx_FLASH.ld")

SET(COMMON_FLAGS "-mcpu=cortex-m0 -mlittle-endian -mthumb")
SET(CMAKE_C_FLAGS "${COMMON_FLAGS} -Wall -Wextra -std=c99 -O0 -ggdb")
SET(CMAKE_EXE_LINKER_FLAGS "${COMMON_FLAGS} -ggdb -Wl,-g -Wl,-gc-sections -T ${LINKER_SCRIPT} -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.map")