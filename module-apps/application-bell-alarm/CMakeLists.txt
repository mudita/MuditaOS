add_library(application-bell-alarm)

target_sources(application-bell-alarm
    PRIVATE
        ApplicationBellAlarm.cpp
        models/BellAlarmWindowModel.cpp
        presenter/BellAlarmWindowPresenter.cpp
        windows/BellAlarmWindow.cpp
    PRIVATE
        data/BellAlarmStyle.hpp
        models/BellAlarmWindowModel.hpp
        presenter/BellAlarmWindowPresenter.hpp
        windows/BellAlarmWindow.hpp
    PUBLIC
        include/application-bell-alarm/ApplicationBellAlarm.hpp
)

target_include_directories(application-bell-alarm
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(application-bell-alarm
    PRIVATE
        apps-common
        module-gui
)
