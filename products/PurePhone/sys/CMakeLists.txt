add_library(sys STATIC)

target_sources(sys
    PRIVATE
        SystemManager.cpp
    PUBLIC
        include/sys/SystemManager.hpp
        include/sys/messages/PhoneModeRequest.hpp
        include/sys/messages/TetheringPhoneModeChangeProhibitedMessage.hpp
)

target_include_directories(sys
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(sys
    PRIVATE
        apps-common
        evtmgr
        module-utils
        module-sys
        service-appmgr
)
