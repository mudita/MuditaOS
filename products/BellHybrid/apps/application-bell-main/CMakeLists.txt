add_library(application-bell-main)

target_sources(application-bell-main
    PRIVATE
        ApplicationBellMain.cpp
        windows/BellHomeScreenWindow.cpp
        windows/BellMainMenuWindow.cpp

        models/AlarmModel.cpp
        models/TemperatureModel.cpp
        models/TimeModel.cpp

        presenters/HomeScreenPresenter.cpp
        presenters/StateController.cpp

        windows/BellHomeScreenWindow.hpp
        windows/BellMainMenuWindow.hpp

        models/AlarmModel.hpp
        models/TemperatureModel.hpp
        models/TimeModel.hpp

        presenters/HomeScreenPresenter.hpp
        presenters/StateController.hpp

    PUBLIC
        include/application-bell-main/ApplicationBellMain.hpp
)

target_include_directories(application-bell-main
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(application-bell-main
    PRIVATE
        application-bell-alarm
        application-bell-settings
        application-bell-powernap
        apps-common
        bellgui
        i18n
        keymap
        module-gui
        service-gui
        time
        bell::evtmgr
        ${TARGET_LIBRARIES}
)
