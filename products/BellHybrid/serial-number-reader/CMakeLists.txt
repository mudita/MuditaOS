add_library(serial-number-reader STATIC)


target_sources(serial-number-reader
    PUBLIC
        SerialNumberReader.cpp
)

target_include_directories(serial-number-reader
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(serial-number-reader
    PRIVATE
    module-vfs
)
