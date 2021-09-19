target_sources(log
    PRIVATE
        log_rt1051.cpp
        time_syscalls.cpp
)

target_link_libraries(log
    PRIVATE
        module-bsp
        segger::rtt
        utils-time
)

if (${LOG_REDIRECT} STREQUAL "RTT_JLINK")
    target_sources(
        log
    PRIVATE
        log_writeRTT.cpp
    )
elseif (${LOG_REDIRECT} STREQUAL "RTT_LUART")
    target_sources(
        log
    PRIVATE    
        log_writeLUART.cpp
    )
elseif ((${LOG_REDIRECT} STREQUAL "RTT_SYSTEMVIEW") AND (${SYSTEM_VIEW_ENABLED} EQUAL 1))
    target_sources(
        log
    PRIVATE
        log_writeSYSTEMVIEW.cpp
    )
endif ()
