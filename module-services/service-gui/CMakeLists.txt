add_library(service-gui STATIC)

target_sources(service-gui
    PRIVATE
        ContextPool.cpp
        DrawCommandsQueue.cpp
        RenderCache.cpp
        ServiceGUI.cpp
        SynchronizationMechanism.cpp
        WorkerGUI.cpp
        messages/DrawMessage.cpp
    INTERFACE
        service-gui/Common.hpp
        service-gui/ServiceGUI.hpp
        service-gui/messages/ChangeColorScheme.hpp
        service-gui/messages/DrawMessage.hpp
        service-gui/messages/EinkInitialized.hpp
        service-gui/messages/GUIMessage.hpp
        service-gui/messages/RenderingFinished.hpp
    PRIVATE
        ContextPool.hpp
        DrawCommandsQueue.hpp
        RenderCache.hpp
        SynchronizationMechanism.hpp
        WorkerGUI.hpp
)


target_link_libraries(service-gui
    PRIVATE
        module-apps
        module-gui
        service-appmgr
        service-desktop
        service-eink
)

target_include_directories(service-gui
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
    PRIVATE
        service-gui
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
