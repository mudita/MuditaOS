add_library(sys-service STATIC)

target_sources(sys-service
    PUBLIC
        include/Timers/SystemTimer.hpp
        include/Timers/TimerFactory.hpp
        include/Timers/Timer.hpp
        include/Timers/TimerMessage.hpp
        include/Timers/TimerHandle.hpp
        include/Service/ServiceManifest.hpp
        include/Service/ServiceCreator.hpp
        include/Service/MessageForward.hpp
        include/Service/BusProxy.hpp
        include/Service/ServiceForward.hpp
        include/Service/Worker.hpp
        include/Service/Service.hpp
        include/Service/ServiceProxy.hpp
        include/Service/Mailbox.hpp
        include/Service/Message.hpp

    PRIVATE
        details/bus/Bus.cpp
        details/bus/Bus.hpp

        BusProxy.cpp
        Message.cpp
        Service.cpp
        SystemTimer.cpp
        TimerFactory.cpp
        TimerHandle.cpp
        Worker.cpp
)

target_include_directories(sys-service
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(sys-service
    PUBLIC
        messagetype
        module-bsp
        module-os
        sys-common
        sys-watchdog
        module-utils

        magic_enum::magic_enum
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
