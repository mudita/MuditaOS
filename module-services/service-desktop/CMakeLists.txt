add_subdirectory(endpoints)
add_subdirectory(message-common)
add_subdirectory(message-sender)

add_library(service-desktop STATIC)

target_sources(
        service-desktop
    PRIVATE
        DesktopEvent.cpp
        DeveloperModeMessage.cpp
        DesktopMessages.cpp
        ServiceDesktop.cpp
        WorkerDesktop.cpp
        USBSecurityModel.cpp
        parser/ParserFSM.cpp
        parser/MessageHandler.cpp
    PRIVATE
        WorkerDesktop.hpp
        parser/ParserFSM.hpp
        parser/MessageHandler.hpp
    PUBLIC
        include/service-desktop/Constants.hpp
        include/service-desktop/DesktopEvent.hpp
        include/service-desktop/DesktopMessages.hpp
        include/service-desktop/DeveloperModeMessage.hpp
        include/service-desktop/ServiceDesktop.hpp
        include/service-desktop/USBSecurityModel.hpp
)

target_include_directories(
        service-desktop
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/parser>
)

add_dependencies(service-desktop version)

target_link_libraries(service-desktop
    PRIVATE
        application-desktop
        module-utils
        service-appmgr
        service-cellular
        service-evtmgr
        utf8
        utils-bootconfig
        Microsoft.GSL::GSL
        json::json
        $<$<STREQUAL:${PROJECT_TARGET},TARGET_RT1051>:usb_stack::usb_stack>
    PUBLIC
        module-cellular
        eventstore
        hash-library::hash-library
        desktop-endpoints
)


if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()

