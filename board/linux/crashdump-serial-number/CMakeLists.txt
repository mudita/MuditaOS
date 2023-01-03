add_library(crashdump-serial-number STATIC)

target_include_directories(crashdump-serial-number
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>/include
)

target_sources(crashdump-serial-number
    PUBLIC
        crashdump_serial_number.cpp
)


