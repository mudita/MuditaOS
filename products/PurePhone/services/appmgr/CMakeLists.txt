add_library(appmgr STATIC)

target_sources(appmgr
    PRIVATE
        ApplicationManager.cpp
        models/WallpaperDisplayModel.cpp
        include/appmgr/models/WallpaperDisplayModel.hpp
    PUBLIC
        include/appmgr/ApplicationManager.hpp
)

target_include_directories(appmgr
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(appmgr
    PRIVATE
        application-desktop
        application-special-input
        apps-common
        module-apps
        module-db
        module-sys
        service-appmgr
        service-cellular
        service-db
        service-desktop
        service-evtmgr
        sys
)
