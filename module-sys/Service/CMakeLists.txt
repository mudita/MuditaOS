add_library(sys-service STATIC)

target_sources(sys-service
    PUBLIC
        include/Service/BusProxy.hpp
        include/Service/Common.hpp
        include/Service/CpuSentinel.hpp
        include/Service/Mailbox.hpp
        include/Service/MessageForward.hpp
        include/Service/Message.hpp
        include/Service/ServiceCreator.hpp
        include/Service/ServiceForward.hpp
        include/Service/Service.hpp
        include/Service/ServiceManifest.hpp
        include/Service/ServiceProxy.hpp
        include/Service/SystemReturnCodes.hpp
        include/Service/Watchdog.hpp
        include/Service/Worker.hpp

    PRIVATE
        details/bus/Bus.cpp
        details/bus/Bus.hpp

        BusProxy.cpp
        CpuSentinel.cpp
        Message.cpp
        Service.cpp
        Worker.cpp
)

target_include_directories(sys-service
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(sys-service
    PUBLIC
        messagetype
        magic_enum::magic_enum
        module-os
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
