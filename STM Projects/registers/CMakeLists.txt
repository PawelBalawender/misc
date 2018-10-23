project(kisaburo)
cmake_minimum_required(VERSION 3.12)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(HEX_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.bin)

add_definitions(-DSTM32F091xC)

set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE USER_SOURCES "src/*.c")

add_library(CMSIS
include/system_stm32f0xx.c
include/startup_stm32f091xc.s src/math/arm_sin_cos_q8.c src/math/_math.h)

include_directories(include)
include_directories(include/CMSIS)

add_executable(${PROJECT_NAME}.elf ${USER_SOURCES} ${LINKER_SCRIPT} src/math/arm_sin_cos_q8.c src/math/_math.h src/configure.c src/configure.h src/common.c)

target_link_libraries(${PROJECT_NAME}.elf CMSIS)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")