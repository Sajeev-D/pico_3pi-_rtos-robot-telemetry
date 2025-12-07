# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# ✨ ADD THIS - Include FreeRTOS from Pico SDK
set(FREERTOS_KERNEL_PATH ${PICO_SDK_PATH}/lib/FreeRTOS-Kernel)
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/library.cmake)

project(pico_freeRTOS C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable
add_executable(pico_freeRTOS 
    pico_freeRTOS.c
    src/imu_driver.c
    src/imu_bare_metal.c
    src/performance_profiler.c
)

pico_enable_stdio_usb(pico_freeRTOS 1)   # Enable USB serial
pico_enable_stdio_uart(pico_freeRTOS 0)  # Disable UART serial

pico_set_program_name(pico_freeRTOS "pico_freeRTOS")
pico_set_program_version(pico_freeRTOS "0.1")

# Enable stdio (you had these disabled - let's enable USB for debugging)
pico_enable_stdio_uart(pico_freeRTOS 0)
pico_enable_stdio_usb(pico_freeRTOS 1)

# Add libraries - ✨ SIMPLIFIED
target_link_libraries(pico_freeRTOS
    pico_stdlib
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
    hardware_i2c
)

# Add include directories
target_include_directories(pico_freeRTOS PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/header
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(pico_freeRTOS)