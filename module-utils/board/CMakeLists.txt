add_library(board STATIC)

if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    target_sources(board
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/linux/log_linux.cpp)

elseif(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    target_sources(board
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/cross/log_rt1051.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cross/time_syscalls.cpp)

    if (${LOG_REDIRECT} STREQUAL "RTT_JLINK")
        target_sources(board
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/cross/log_writeRTT.cpp)
    elseif (${LOG_REDIRECT} STREQUAL "RTT_LUART")
        target_sources(board
        PRIVATE    
        ${CMAKE_CURRENT_SOURCE_DIR}/cross/log_writeLUART.cpp)
    elseif ((${LOG_REDIRECT} STREQUAL "RTT_SYSTEMVIEW") AND (${SYSTEM_VIEW_ENABLED} EQUAL 1))
        target_sources(board
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/cross/log_writeSYSTEMVIEW.cpp)
    endif ()
endif()

target_link_libraries(board PUBLIC module-bsp)