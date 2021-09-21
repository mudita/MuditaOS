add_library(utility STATIC)

target_sources(utility
    PRIVATE
        Utils.cpp

    PUBLIC
        integer.hpp
        Split.hpp
        split_sv.hpp
        Temperature.hpp
        Utility.hpp
        Utils.hpp
)

target_include_directories(utility
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

target_link_libraries(utility
    PUBLIC
        log-api
        magic_enum::magic_enum

    PRIVATE
        hash-library::hash-library
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()

