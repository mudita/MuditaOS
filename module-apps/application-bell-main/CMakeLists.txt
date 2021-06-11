add_library(application-bell-main INTERFACE)

target_sources(application-bell-main
    INTERFACE
        ApplicationBellMain.cpp
        windows/BellMainWindow.cpp
)
