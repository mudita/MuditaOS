add_library(application-bell-clock)
add_library(bell::app-clock ALIAS application-bell-clock)

target_sources(application-bell-clock
    PRIVATE
        ApplicationBellClock.cpp
        presenter/BellClockWindowPresenter.cpp
        presenter/SettingsPresenter.cpp
        models/DeepRefreshInterval.cpp
        windows/BellClockWindow.cpp
        windows/BellRefreshClockWindow.cpp
    PRIVATE
        presenter/BellClockWindowPresenter.hpp
        presenter/SettingsPresenter.hpp
        windows/BellClockWindow.hpp
        windows/BellRefreshClockWindow.hpp
        data/Contract.hpp
    PUBLIC
        include/application-bell-clock/ApplicationBellClock.hpp
)

target_include_directories(application-bell-clock
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/application-bell-clock>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(application-bell-clock
    PRIVATE
        app
        bell::app-common
        bell::db
        date::date
        service-appmgr
    PUBLIC
        module-gui
        bell::app-main
        bell::app-common
)
