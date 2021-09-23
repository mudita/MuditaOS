add_library(application-bell-bedtime)
add_library(bell::app-bedtime ALIAS application-bell-bedtime)

target_sources(application-bell-bedtime
    PRIVATE
        ApplicationBellBedtime.cpp
        models/BedtimeListItemProvider.cpp
        presenter/BellBedtimeWindowPresenter.cpp
        windows/BellBedtimeWindow.cpp
    PRIVATE
        data/BellBedtimeStyle.hpp
        models/BedtimeListItemProvider.hpp
        presenter/BellBedtimeWindowPresenter.hpp
        windows/BellBedtimeWindow.hpp
    PUBLIC
        include/application-bell-bedtime/ApplicationBellBedtime.hpp
)

target_include_directories(application-bell-bedtime
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/application-bell-bedtime>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(application-bell-bedtime
    PRIVATE
        app
        bellgui
        bell::app-common
        bell::db
        date::date
        service-appmgr
    PUBLIC
        module-gui
        bell::app-common
)
