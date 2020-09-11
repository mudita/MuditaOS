
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
        windows/BtWindow.cpp
        windows/BtScanWindow.cpp

        windows/UITestWindow.cpp
        windows/Info.cpp
        windows/SimSelectWindow.cpp
        windows/DateTimeWindow.cpp
        windows/CellularPassthroughWindow.cpp
        windows/FotaWindow.cpp
        windows/Fota.cpp
        windows/SettingsChange.cpp

    PUBLIC
        ApplicationSettings.hpp
        windows/SettingsMainWindow.hpp
        windows/LanguageWindow.hpp
        windows/DateTimeWindow.hpp
)

add_dependencies(${PROJECT_NAME} version)
