# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.27)
set(PICO_BOARD pico)
# set(PICO_EXTRAS_PATH /Users/gilbert/Documents/dev/pico/pico-extras)

# Include build functions from Pico SDK
include(pico_sdk_import.cmake)
# include(/Users/gilbert/Documents/dev/pico/pico-extras/external/pico_extras_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C   standards
project(spm4650 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INSTALL_PREFIX /Users/gilbert/Documents/dev/gcc/spm4650)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

add_subdirectory(lib)

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME} 
    main.c
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    hardware_uart
    haw_spm4650
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)