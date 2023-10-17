if(${PROF_ON})
add_catch2_executable(
    NAME performance
    SRCS
        performance-monitor.cpp
    LIBS
        module-os
)
endif()
