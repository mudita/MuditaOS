add_library(application-bell-settings STATIC)

target_include_directories(application-bell-settings
    PRIVATE
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}
            models
            widgets
            windows
        >
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-bell-settings
    PRIVATE
        ApplicationBellSettings.cpp
        models/TimeUnitsModel.cpp
        widgets/TimeSetSpinner.cpp
        windows/BellSettingsWindow.cpp
        windows/BellSettingsAdvancedWindow.cpp
        windows/BellSettingsTimeUnitsWindow.cpp

    PUBLIC
        include/application-bell-settings/ApplicationBellSettings.hpp
        models/TimeUnitsModel.hpp
        widgets/TimeSetSpinner.hpp
        windows/BellSettingsWindow.hpp
        windows/BellSettingsAdvancedWindow.hpp
        windows/BellSettingsTimeUnitsWindow.hpp
)

target_link_libraries(application-bell-settings
    PRIVATE
        bellgui
        service-appmgr
        time
    PUBLIC
        apps-common
        module-gui
)
