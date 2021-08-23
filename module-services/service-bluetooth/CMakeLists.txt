project(service-bluetooth)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )


set(SOURCES
    ServiceBluetooth.cpp
    service-bluetooth/SettingsHolder.cpp
    service-bluetooth/SettingsSerializer.cpp
    service-bluetooth/BluetoothMessage.cpp
    service-bluetooth/BluetoothDevicesModel.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})


target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        module-bluetooth
        service-desktop
        module-sys
        json::json
)
