project(service-appmgr)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
    ApplicationManifest.cpp
    Controller.cpp
    data/MmiActionsParams.cpp
    data/NotificationsChangedActionsParams.cpp
    data/CallActionsParams.cpp
    model/ApplicationManager.cpp
    model/ApplicationHandle.cpp
    model/ApplicationsRegistry.cpp
    model/ActionsRegistry.cpp
    model/ApplicationStack.cpp
    messages/ActionRequest.cpp
    messages/FinishRequest.cpp
    messages/ApplicationCloseRequest.cpp
    messages/ApplicationInitialised.cpp
    messages/ApplicationStatus.cpp
    messages/BaseMessage.cpp
    messages/CloseConfirmation.cpp
    messages/LanguageChangeRequest.cpp
    messages/PowerSaveModeInitRequest.cpp
    messages/PreventBlockingRequest.cpp
    messages/ShutdownRequest.cpp
    messages/SwitchBackRequest.cpp
    messages/SwitchConfirmation.cpp
    messages/SwitchRequest.cpp
    messages/UpdateInProgress.cpp
    messages/DOMRequest.cpp
    messages/SetOsUpdateVersion.cpp
    messages/StartAllowedMessage.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        service-cellular
        service-db
        service-gui
        module-audio
        json::json
    PUBLIC
        apps-common
        time
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
