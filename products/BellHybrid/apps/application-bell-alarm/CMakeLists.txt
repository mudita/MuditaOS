add_library(application-bell-alarm)
add_library(bell::app-alarm ALIAS application-bell-alarm)

target_sources(application-bell-alarm
    PRIVATE
        ApplicationBellAlarm.cpp
        presenter/BellAlarmWindowPresenter.cpp
        presenter/BellAlarmSetPresenter.cpp
        windows/BellAlarmWindow.cpp
        windows/BellAlarmSetWindow.cpp

        data/BellAlarmStyle.hpp
        presenter/BellAlarmWindowPresenter.hpp
        presenter/BellAlarmSetPresenter.hpp
        windows/BellAlarmWindow.hpp
        windows/BellAlarmSetWindow.hpp
    PUBLIC
        include/application-bell-alarm/ApplicationBellAlarm.hpp
)

target_include_directories(application-bell-alarm
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/application-bell-alarm>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(application-bell-alarm
    PRIVATE
        app
        bell::appmgr
        bell::app-main
        bell::app-common
        module-gui
)
