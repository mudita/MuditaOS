# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

project(module-bluetooth VERSION 1.0 DESCRIPTION "Bluetooth module library")

set(CMAKE_CXX_STANDARD 17)

module_is_test_entity()

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/audio/BluetoothAudioDevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/BluetoothWorker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/error_bluetooth.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/WorkerController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/CommandHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/Device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/glucode/BluetoothRunLoop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/BluetoothDriverImpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/BtKeysStorage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/GAP/GAP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/GAP/Devices.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/GAP/used_events.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/A2DP/A2DP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/A2DP/AVRCP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/A2DP/AVDTP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/HSP/HSP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/SCO/SCO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/SCO/ScoUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/HFP/HFP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/Profile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/ProfileManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/interface/profiles/PhoneInterface.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/command/Command.cpp
        )

if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    add_subdirectory(Bluetooth/doc/uml/)
endif()



include(lib/btstack.cmake)
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_DIR_SOURCES})


# # Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC   ${PROJECT_CONFIG_DEFINITIONS}
                                                    ${PROJECT_TARGET}
                                                    ${BOARD_DIR_DEFINITIONS}
                                                    )
target_compile_options(${PROJECT_NAME}
    PRIVATE
    -Wno-sign-compare
    -Wno-missing-field-initializers
    -Wno-unused-function
    -Wno-implicit-fallthrough
    # C only flags
    "$<$<COMPILE_LANGUAGE:C>:-Wno-old-style-declaration>"
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${TARGET_LIBRARIES_INCLUDES}
    ${BT_STACK_ROOT}
    ${BT_STACK_ROOT}/src
    ${BT_STACK_ROOT}/src/classic
)

target_link_libraries(${PROJECT_NAME}
    $<$<BOOL:${ENABLE_APP_SETTINGS}>:application-settings>
    ${BOARD_DIR_LIBRARIES}
    module-audio
    module-bsp
    module-sys
    module-utils
    module-vfs
    service-audio
    service-bluetooth
    service-evtmgr
    sml::sml
    sml::utils::logger
    json::json
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
