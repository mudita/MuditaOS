add_library(serial-number-parser STATIC)


target_sources(serial-number-parser
    PUBLIC
        SerialNumberParser.cpp
        DeviceMetadata.cpp
)

target_include_directories(serial-number-parser
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(serial-number-parser
    PRIVATE
    module-vfs
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
