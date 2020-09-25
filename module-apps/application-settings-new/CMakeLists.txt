
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
        windows/BluetoothWindow.cpp
        windows/BaseSettingsWindow.cpp
        windows/FontSizeWindow.cpp
        windows/InputLanguageWindow.cpp
        windows/DisplayLightWindow.cpp
        windows/KeypadLightWindow.cpp
        windows/LockedScreenWindow.cpp
        windows/DisplayAndKeypadWindow.cpp
        windows/NetworkWindow.cpp

    PUBLIC
        ApplicationSettings.hpp
        windows/SettingsMainWindow.hpp
        windows/BaseSettingsWindow.hpp
        windows/FontSizeWindow.hpp
        windows/InputLanguageWindow.hpp
        windows/DisplayLightWindow.hpp
        windows/KeypadLightWindow.hpp
        windows/LockedScreenWindow.hpp
        windows/DisplayAndKeypadWindow.hpp
)

add_dependencies(${PROJECT_NAME} version)
