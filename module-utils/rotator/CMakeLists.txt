add_library(utils-rotator INTERFACE)

target_sources(utils-rotator
    PUBLIC
        include/rotator/Rotator.hpp
)

target_include_directories(utils-rotator
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
