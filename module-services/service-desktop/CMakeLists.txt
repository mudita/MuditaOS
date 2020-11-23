project(service-desktop)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
    endpoints/backup/BackupEndpoint.cpp
    endpoints/backup/BackupRestore.cpp
    endpoints/calllog/CalllogEndpoint.cpp
    endpoints/calllog/CalllogHelper.cpp
    endpoints/contacts/ContactHelper.cpp
    endpoints/contacts/ContactsEndpoint.cpp
    endpoints/developerMode/DeveloperModeEndpoint.cpp
    endpoints/developerMode/DeveloperModeHelper.cpp
    endpoints/deviceInfo/DeviceInfoEndpoint.cpp
    endpoints/deviceUpdate/DeviceUpdateEndpoint.cpp
    endpoints/factoryReset/FactoryReset.cpp
    endpoints/factoryReset/FactoryResetEndpoint.cpp
    endpoints/messages/MessageHelper.cpp
    endpoints/messages/MessagesEndpoint.cpp
    endpoints/restore/RestoreEndpoint.cpp
    endpoints/update/UpdateEndpoint.cpp
    endpoints/update/UpdateMuditaOS.cpp
    endpoints/filesystem/FilesystemEndpoint.cpp

    parser/ParserUtils.cpp
    parser/ParserFSM.cpp
    parser/MessageHandler.cpp

    DesktopMessages.cpp
    ServiceDesktop.cpp
    WorkerDesktop.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

add_dependencies(${PROJECT_NAME} version)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        service-cellular
        service-evtmgr
        module-utils
        module-cellular
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()

