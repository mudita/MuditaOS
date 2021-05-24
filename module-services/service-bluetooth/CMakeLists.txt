project(service-bluetooth)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )


set(SOURCES
    ServiceBluetooth.cpp
    service-bluetooth/SettingsHolder.cpp
    service-bluetooth/SettingsSerializer.cpp
    service-bluetooth/BluetoothMessage.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})


target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        module-apps
        module-bluetooth
        module-db
        module-gui
        module-sys
        service-appmgr
        service-cellular
        service-db
        service-desktop
)
