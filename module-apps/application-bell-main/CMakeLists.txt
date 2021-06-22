add_library(application-bell-main STATIC)

target_sources(application-bell-main
    PRIVATE
        ApplicationBellMain.cpp
        windows/BellMainWindow.cpp
        windows/BellMainMenuWindow.cpp
    PRIVATE
        windows/BellMainWindow.hpp
        windows/BellMainMenuWindow.hpp
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
        ${TARGET_LIBRARIES}
)
