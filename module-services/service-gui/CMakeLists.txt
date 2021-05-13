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
        service-guiName.hpp
        ContextPool.hpp
        DrawCommandsQueue.hpp
        RenderCache.hpp
        ServiceGUI.hpp
        SynchronizationMechanism.hpp
        WorkerGUI.hpp
        messages/ChangeColorScheme.hpp
        messages/DrawMessage.hpp
        messages/EinkInitialized.hpp
        messages/GUIMessage.hpp
        messages/RenderingFinished.hpp
)


target_link_libraries(service-gui
    service-eink
    module-gui
    module-apps
)

target_include_directories(service-gui
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/messages
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
