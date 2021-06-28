project(service-desktop)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
    endpoints/BaseHelper.cpp
    endpoints/backup/BackupEndpoint.cpp
    endpoints/backup/BackupRestore.cpp
    endpoints/bluetooth/BluetoothEndpoint.cpp
    endpoints/bluetooth/BluetoothHelper.cpp
    endpoints/bluetooth/BluetoothEventMessages.cpp
    endpoints/bluetooth/BluetoothMessagesHandler.cpp
    endpoints/calllog/CalllogEndpoint.cpp
    endpoints/calllog/CalllogHelper.cpp
    endpoints/contacts/ContactHelper.cpp
    endpoints/contacts/ContactsEndpoint.cpp
    endpoints/developerMode/DeveloperModeEndpoint.cpp
    endpoints/developerMode/DeveloperModeHelper.cpp
    endpoints/developerMode/Mode/UI_Helper.cpp
    endpoints/developerMode/event/DomRequest.cpp
    endpoints/developerMode/event/ATRequest.cpp
    endpoints/deviceInfo/DeviceInfoEndpoint.cpp
    endpoints/deviceInfo/DeviceInfoEndpointHelper.cpp
    endpoints/factoryReset/FactoryReset.cpp
    endpoints/factoryReset/FactoryResetEndpoint.cpp
    endpoints/messages/MessageHelper.cpp
    endpoints/messages/MessagesEndpoint.cpp
    endpoints/restore/RestoreEndpoint.cpp
    endpoints/update/UpdateEndpoint.cpp
    endpoints/update/UpdateMuditaOS.cpp
    endpoints/filesystem/FilesystemEndpoint.cpp
    endpoints/filesystem/FileOperations.cpp
    endpoints/filesystem/FileContext.cpp
    endpoints/calendarEvents/CalendarEventsHelper.cpp
    endpoints/calendarEvents/CalendarEventsEndpoint.cpp
    endpoints/security/SecurityEndpoint.cpp
    endpoints/security/SecurityEndpointHelper.cpp

    parser/HttpEnums.cpp
    parser/ParserFSM.cpp
    parser/MessageHandler.cpp

    DesktopEvent.cpp
    DeveloperModeMessage.cpp
    DesktopMessages.cpp
    ServiceDesktop.cpp
    WorkerDesktop.cpp
    USBSecurityModel.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

add_dependencies(${PROJECT_NAME} version)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        microtar
        module-utils
        service-cellular
        service-evtmgr
        utf8
        utils-bootconfig
        Microsoft.GSL::GSL
        json::json
        base64::base64
        $<$<STREQUAL:${PROJECT_TARGET},TARGET_RT1051>:usb_stack::usb_stack>
    PUBLIC
        module-cellular
        eventstore
        hash-library::hash-library
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()

