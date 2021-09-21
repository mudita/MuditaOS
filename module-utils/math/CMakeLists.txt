add_library(utils-math INTERFACE)

target_include_directories(utils-math
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
