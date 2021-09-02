add_library(log STATIC)

add_subdirectory(api)
add_subdirectory(board)

target_sources(log
    PRIVATE
        Logger.cpp
        log.cpp
        LoggerBuffer.cpp
        StringCircularBuffer.cpp
)

target_include_directories(log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(log
    PRIVATE
        utility
        purefs-paths

    PUBLIC
        module-os
        log-api
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
