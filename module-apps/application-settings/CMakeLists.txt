
include_directories( ${CMAKE_PROJECT_NAME}

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
)

include_directories( ${PROJECT_NAME}

    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_sources( ${PROJECT_NAME}

    PRIVATE
        ApplicationSettings.cpp
        windows/SettingsMainWindow.cpp
        windows/LanguageWindow.cpp

        windows/UITestWindow.cpp
        windows/Info.cpp
        windows/SimSelectWindow.cpp
        windows/DateTimeWindow.cpp
        windows/CellularPassthroughWindow.cpp
        windows/FotaWindow.cpp
        windows/Fota.cpp
        windows/SettingsChange.cpp
        windows/BaseSettingsWindow.cpp
        windows/display-and-keypad/DisplayAndKeypadWindow.cpp
        windows/display-and-keypad/DisplayLightWindow.cpp
        windows/display-and-keypad/FontSizeWindow.cpp
        windows/display-and-keypad/InputLanguageWindow.cpp
        windows/display-and-keypad/KeypadLightWindow.cpp
        windows/display-and-keypad/LockedScreenWindow.cpp

    PUBLIC
        ApplicationSettings.hpp
        windows/SettingsMainWindow.hpp
        windows/LanguageWindow.hpp
        windows/DateTimeWindow.hpp
        windows/BaseSettingsWindow.hpp
        windows/display-and-keypad/DisplayAndKeypadWindow.hpp
        windows/display-and-keypad/DisplayLightWindow.hpp
        windows/display-and-keypad/FontSizeWindow.hpp
        windows/display-and-keypad/InputLanguageWindow.hpp
        windows/display-and-keypad/KeypadLightWindow.hpp
        windows/display-and-keypad/LockedScreenWindow.hpp
)

add_dependencies(${PROJECT_NAME} version)
