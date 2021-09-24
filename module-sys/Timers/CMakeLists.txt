add_library(sys-timers STATIC)

target_sources(sys-timers
    PUBLIC
        include/Timers/SystemTimer.hpp
        include/Timers/TimerFactory.hpp
        include/Timers/Timer.hpp
        include/Timers/TimerMessage.hpp
        include/Timers/TimerHandle.hpp

    PRIVATE
        SystemTimer.cpp
        TimerFactory.cpp
        TimerHandle.cpp
)

target_include_directories(sys-timers
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(sys-timers
    PUBLIC
        sys-service
        module-os
)
