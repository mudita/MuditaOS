
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

    PUBLIC
        ApplicationSettings.hpp
        windows/SettingsMainWindow.hpp
)

add_dependencies(${PROJECT_NAME} version)
