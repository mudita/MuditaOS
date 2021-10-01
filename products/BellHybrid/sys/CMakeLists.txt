add_library(sys STATIC)

target_sources(sys
    PRIVATE
        SystemManager.cpp
    PUBLIC
        include/sys/messages/AlarmActivationStatusChangeRequest.hpp
        include/sys/SystemManager.hpp
)

target_include_directories(sys
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(sys
    PRIVATE
        bell::appmgr
        module-sys
)
