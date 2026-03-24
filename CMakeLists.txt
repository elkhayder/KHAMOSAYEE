# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
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
# ====================================================================================
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

project(khamosayee C CXX ASM)

pico_sdk_init()

add_executable(khamosayee
    ${CMAKE_CURRENT_LIST_DIR}/main.c
    ${CMAKE_CURRENT_LIST_DIR}/cmps12.c
    ${CMAKE_CURRENT_LIST_DIR}/servo_direction.c
    ${CMAKE_CURRENT_LIST_DIR}/servo_voile.c
    ${CMAKE_CURRENT_LIST_DIR}/pid.c
)

pico_set_program_name(khamosayee "khamosayee")
pico_set_program_version(khamosayee "0.1")

pico_enable_stdio_uart(khamosayee 0)
pico_enable_stdio_usb(khamosayee 1)

target_include_directories(khamosayee PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(khamosayee
    pico_stdlib
    hardware_i2c
    hardware_pwm
)

pico_add_extra_outputs(khamosayee)