add_library(logdump STATIC)

target_sources(logdump
    PRIVATE
        logdump.cpp
    PUBLIC
        include/logdump/logdump.h
)

target_include_directories(logdump
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(logdump
    PRIVATE
        purefs-paths
        log
)
