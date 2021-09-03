add_library(application-bell-alarm)
add_library(bell::app-alarm ALIAS application-bell-alarm)

target_sources(application-bell-alarm
    PRIVATE
        ApplicationBellAlarm.cpp
        presenter/BellAlarmWindowPresenter.cpp
        windows/BellAlarmWindow.cpp
    PRIVATE
        data/BellAlarmStyle.hpp
        presenter/BellAlarmWindowPresenter.hpp
        windows/BellAlarmWindow.hpp
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
        bell::app-common
        module-gui
)
