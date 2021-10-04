add_library(appmgr STATIC)
add_library(bell::appmgr ALIAS appmgr)

target_sources(appmgr
    PRIVATE
        ApplicationManager.cpp
    PUBLIC
        include/appmgr/messages/AlarmMessage.hpp
        include/appmgr/ApplicationManager.hpp
        include/appmgr/messages/PowerOffPopupRequestParams.hpp
        include/appmgr/messages/RebootPopupRequestParams.hpp
)

target_include_directories(appmgr
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(appmgr
   PRIVATE
        module-apps
        application-bell-onboarding
        service-appmgr
   PUBLIC
        alarms
)
