option(ENABLE_DEVELOPER_MODE_ENDPOINT "Enable developer mode endpoint in service desktop" ${ENABLE_DEVELOPER_MODE_ENDPOINT_DEFAULT})

add_library(desktop-endpoint-base STATIC)

target_sources(
        desktop-endpoint-base
    PRIVATE
        BaseHelper.cpp
        Endpoint.cpp
        HttpEnums.cpp
    PUBLIC
        include/endpoints/BaseHelper.hpp
        include/endpoints/Context.hpp
        include/endpoints/DBHelper.hpp
        include/endpoints/Endpoint.hpp
        include/endpoints/EndpointType.hpp
        include/endpoints/EndpointFactory.hpp
        include/endpoints/HttpEnums.hpp
        include/endpoints/JsonKeyNames.hpp
        include/endpoints/ResponseContext.hpp
)

target_include_directories(
        desktop-endpoint-base
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(
        desktop-endpoint-base
    PUBLIC
        endpoint-message-sender
        hash-library::hash-library
        module-db
        sys-service
)

add_library(desktop-endpoints-common STATIC)

target_sources(
        desktop-endpoints-common
    PRIVATE
        backup/BackupEndpoint.cpp
        backup/BackupHelper.cpp
        bluetooth/BluetoothEndpoint.cpp
        bluetooth/BluetoothEventMessages.cpp
        bluetooth/BluetoothHelper.cpp
        bluetooth/BluetoothMessagesHandler.cpp
        developerMode/DeveloperModeEndpoint.cpp
        developerMode/DeveloperModeHelper.cpp
        developerMode/Mode/UI_Helper.cpp
        developerMode/event/ATRequest.cpp
        developerMode/event/DomRequest.cpp
        developerMode/fs/FS_Helper.cpp
        factoryReset/FactoryResetEndpoint.cpp
        filesystem/FileContext.cpp
        filesystem/FileOperations.cpp
        filesystem/FilesystemEndpoint.cpp
        nullEndpoint/NullEndpoint.cpp
        restore/RestoreEndpoint.cpp
        security/SecurityEndpoint.cpp
        security/SecurityEndpointHelper.cpp
        update/UpdateEndpoint.cpp
        update/UpdateHelper.cpp
    PUBLIC
        include/endpoints/backup/BackupEndpoint.hpp
        include/endpoints/backup/BackupHelper.hpp
        include/endpoints/bluetooth/BluetoothEndpoint.hpp
        include/endpoints/bluetooth/BluetoothEventMessages.hpp
        include/endpoints/bluetooth/BluetoothHelper.hpp
        include/endpoints/bluetooth/BluetoothMessagesHandler.hpp
        include/endpoints/developerMode/DeveloperModeEndpoint.hpp
        include/endpoints/developerMode/DeveloperModeHelper.hpp
        include/endpoints/developerMode/Mode/UI_Helper.hpp
        include/endpoints/developerMode/event/ATRequest.hpp
        include/endpoints/developerMode/event/DomRequest.hpp
        include/endpoints/developerMode/fs/FS_Helper.hpp
        include/endpoints/factoryReset/FactoryResetEndpoint.hpp
        include/endpoints/filesystem/FileContext.hpp
        include/endpoints/filesystem/FileOperations.hpp
        include/endpoints/filesystem/FilesystemEndpoint.hpp
        include/endpoints/nullEndpoint/NullEndpoint.hpp
        include/endpoints/restore/RestoreEndpoint.hpp
        include/endpoints/security/SecurityEndpoint.hpp
        include/endpoints/security/SecurityEndpointHelper.hpp
        include/endpoints/update/UpdateEndpoint.hpp
        include/endpoints/update/UpdateHelper.hpp
)

target_include_directories(
        desktop-endpoints-common
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(
        desktop-endpoints-common
    PUBLIC
        desktop-endpoint-base
    PRIVATE
        microtar
        base64::base64
)

add_library(desktop-endpoints INTERFACE)

target_link_libraries(
        desktop-endpoints
    INTERFACE
        desktop-endpoints-product
)
