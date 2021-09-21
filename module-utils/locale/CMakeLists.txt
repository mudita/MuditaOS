add_library(utils-locale INTERFACE)

target_sources(utils-locale INTERFACE country.hpp)
target_include_directories(utils-locale
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)
