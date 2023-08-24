# Include source file(s) to project
add_library(haw_spm4650 STATIC
    spm_srxl.c
)

# Links all required dependencies
target_link_libraries(haw_spm4650
    pico_stdlib
    hardware_uart 
)

# Link include directory / Path to header file
target_include_directories(haw_spm4650 PUBLIC ../include)