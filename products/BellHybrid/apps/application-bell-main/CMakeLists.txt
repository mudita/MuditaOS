add_library(application-bell-main STATIC)
add_library(bell::app-main ALIAS application-bell-main)

target_sources(application-bell-main
    PRIVATE
        ApplicationBellMain.cpp

        windows/BellBatteryShutdownWindow.cpp
        windows/BellHomeScreenWindow.cpp
        windows/BellMainMenuWindow.cpp
        windows/BellBatteryStatusWindow.cpp

        models/BatteryModel.cpp
        models/TemperatureModel.cpp

        presenters/HomeScreenPresenter.cpp
        presenters/StateController.cpp

        windows/BellBatteryShutdownWindow.hpp
        windows/BellHomeScreenWindow.hpp
        windows/BellMainMenuWindow.hpp
        windows/BellBatteryStatusWindow.hpp

        models/BatteryModel.cpp
        models/TemperatureModel.hpp

        presenters/StateController.hpp

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
        bell::app-background-sounds
        bell::app-bedtime
        bell::app-meditation-timer
        bell::app-settings
        bell::app-powernap
        bell::keymap
        ${TARGET_LIBRARIES}
    PUBLIC
        bell::app-common
        sml::sml
        sml::utils::logger
)
