add_library(application-bell-main STATIC)
add_library(bell::app-main ALIAS application-bell-main)

target_sources(application-bell-main
    PRIVATE
        ApplicationBellMain.cpp

        windows/BellBatteryShutdownWindow.cpp
        windows/BellHomeScreenWindow.cpp
        windows/BellMainMenuWindow.cpp
        windows/BellMainVolumeWindow.cpp

        models/TemperatureModel.cpp
        models/UsbStatusModel.cpp
        models/DemoModel.cpp
        models/MainFrontlightModel.cpp

        presenters/HomeScreenPresenter.cpp
        presenters/StateController.cpp
        presenters/BellMainVolumePresenter.cpp

        windows/BellBatteryShutdownWindow.hpp
        windows/BellHomeScreenWindow.hpp
        windows/BellMainMenuWindow.hpp
        windows/BellMainVolumeWindow.hpp

        models/TemperatureModel.hpp
        models/UsbStatusModel.hpp
        models/MainFrontlightModel.hpp

        presenters/StateController.hpp
        presenters/BellMainVolumePresenter.hpp

    PUBLIC
        include/application-bell-main/ApplicationBellMain.hpp
        include/application-bell-main/presenters/HomeScreenPresenter.hpp
)

target_include_directories(application-bell-main
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(application-bell-main
    PRIVATE
        alarms
        app
        apps-common
        i18n
        module-gui
        service-gui
        utils-time
        bell::appmgr
        bell::db
        bell::evtmgr
        bell::app-alarm
        bell::app-onboarding
        bell::app-relaxation
        bell::app-bedtime
        bell::app-meditation-timer
        bell::app-focus-timer
        bell::app-settings
        bell::app-powernap
        bell::keymap
        bell::paths
        ${TARGET_LIBRARIES}
    PUBLIC
        bell::app-common
        sml::sml
        sml::utils::logger
)
