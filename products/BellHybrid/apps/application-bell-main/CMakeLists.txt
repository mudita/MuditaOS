add_library(application-bell-main STATIC)
add_library(bell::app-main ALIAS application-bell-main)

target_sources(application-bell-main
    PRIVATE
        ApplicationBellMain.cpp
        windows/BellHomeScreenWindow.cpp
        windows/BellMainMenuWindow.cpp

        models/TemperatureModel.cpp
        models/TimeModel.cpp

        presenters/HomeScreenPresenter.cpp
        presenters/StateController.cpp

        windows/BellHomeScreenWindow.hpp
        windows/BellMainMenuWindow.hpp

        models/TemperatureModel.hpp
        models/TimeModel.hpp

        presenters/HomeScreenPresenter.hpp
        presenters/StateController.hpp
        presenters/StateControllerLogger.hpp

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
        apps-common
        bellgui
        i18n
        module-gui
        service-gui
        time
        bell::db
        bell::evtmgr
        bell::app-alarm
        bell::app-settings
        bell::app-powernap
        bell::keymap
        ${TARGET_LIBRARIES}
    PUBLIC
        bell::app-common
)
