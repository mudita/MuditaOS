add_library(log STATIC)

add_subdirectory(api)
add_subdirectory(board)
add_subdirectory(logdump)

target_sources(log
    PRIVATE
        Logger.cpp
        log.cpp
        LoggerBuffer.cpp
        LoggerWorker.cpp
        StringCircularBuffer.cpp
)

target_include_directories(log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(log
    PRIVATE
        utility
        purefs-paths
        crashdump-metadata-store
        Microsoft.GSL::GSL
    PUBLIC
        module-os
        log-api
        utils-rotator
        sys-service
)

if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    if(${PRODUCT} STREQUAL "PurePhone")
        set(CIRCULAR_BUFFER_SIZE 1024)
    elseif(${PRODUCT} STREQUAL "BellHybrid")
        set(CIRCULAR_BUFFER_SIZE 512)
    else()
        message(FATAL_ERROR "Unknown product: ${PRODUCT}")
    endif()
elseif (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    set(CIRCULAR_BUFFER_SIZE 1024)
else()
    message(FATAL_ERROR "Unknown target: ${PROJECT_TARGET}")
endif()

target_compile_definitions(log
        PUBLIC
        LOGGER_CIRCULAR_BUFFER_SIZE=${CIRCULAR_BUFFER_SIZE}
        )

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
