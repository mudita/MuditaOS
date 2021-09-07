add_library(application-bell-common STATIC)
add_library(bell::app-common ALIAS application-bell-common)

target_include_directories(application-bell-common
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/common>
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )

target_sources(application-bell-common
        PRIVATE
        src/AlarmModel.cpp

        PUBLIC
        include/common/models/AlarmModel.hpp
        include/common/models/AbstractAlarmModel.hpp
        )


target_link_libraries(application-bell-common
        PUBLIC
        apps-common

        PRIVATE
        module-gui
        service-time

        )
