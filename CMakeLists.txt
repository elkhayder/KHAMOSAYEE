cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(imu_data C CXX ASM)

pico_sdk_init()

add_executable(imu_data imu_data.c)

add_executable(servomotor_direction servomotor_direction.c)

pico_set_program_name(imu_data "imu_data")
pico_set_program_version(imu_data "0.1")

pico_set_program_name(servomotor_direction "servomotor_direction")
pico_set_program_version(servomotor_direction "0.1")

pico_enable_stdio_uart(imu_data 0)
pico_enable_stdio_usb(imu_data 1)

pico_enable_stdio_uart(servomotor_direction 0)
pico_enable_stdio_usb(servomotor_direction 1)

target_include_directories(imu_data PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_include_directories(servomotor_direction PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(imu_data
        pico_stdlib
        hardware_i2c
        hardware_pwm
)

target_link_libraries(servomotor_direction
    pico_stdlib
    hardware_pwm
)

pico_add_extra_outputs(imu_data)

pico_add_extra_outputs(servomotor_direction)