project(module-bluetooth VERSION 1.0 DESCRIPTION "Bluetooth module library")

include(${CMAKE_SOURCE_DIR}/config/ModuleConfig.cmake)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/BluetoothWorker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/WorkerController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/CommandHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/glucode/BluetoothRunLoop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/BluetoothDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/BtKeysStorage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/GAP/GAP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/A2DP/A2DP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/A2DP/AVRCP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/A2DP/AVDTP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/HSP/HSP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/HSP/SCO.cpp
)

message("Build with BlueKitchen")
include(lib/btstack.cmake)

message(${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
message("Sources: " ${SOURCES})
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_DIR_SOURCES})


# # Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC   ${PROJECT_CONFIG_DEFINITIONS}
                                                    ${PROJECT_TARGET}
                                                    ${TARGET_COMPILE_DEFINITIONS}
                                                    ${BOARD_DIR_DEFINITIONS}
                                                    )
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME}
    PUBLIC
    ${TARGET_COMPILE_OPTIONS}
    PRIVATE
    -Wno-sign-compare
    -Wno-missing-field-initializers
    -Wno-unused-function
    -Wno-implicit-fallthrough

    # C only flags
    "$<$<COMPILE_LANGUAGE:C>:-Wno-old-style-declaration>"
)
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${BOARD_DIR_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${TARGET_LIBRARIES_INCLUDES}
    ${BT_STACK_ROOT}/src
    ${BT_STACK_ROOT}/src/classic


)

target_link_libraries(${PROJECT_NAME}
    service-evtmgr
    module-bsp
    service-bluetooth
    module-utils
    module-vfs
    module-sys
    module-audio
    service-audio
    ${BOARD_DIR_LIBRARIES}
    )

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
