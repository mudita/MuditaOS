add_library(appmgr STATIC)

target_sources(appmgr
    PRIVATE
        ApplicationManager.cpp
    PUBLIC
        include/appmgr/messages/AlarmMessage.hpp
        include/appmgr/ApplicationManager.hpp
)

target_include_directories(appmgr
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(appmgr
   PRIVATE
        module-apps
        service-appmgr
   PUBLIC
        alarms
)
