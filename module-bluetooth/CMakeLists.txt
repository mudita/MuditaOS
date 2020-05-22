project(module-bluetooth VERSION 1.0 DESCRIPTION "Bluetooth module library")

include(${CMAKE_SOURCE_DIR}/config/ModuleConfig.cmake)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Bluetooth/BluetoothWorker.cpp
)

if(${BT_STACK} STREQUAL "Bluetopia")
    if(NOT ${PROJECT_TARGET} STREQUAL "TARGET_Linux")
        message("Build with bluetopia")
        include(lib/Bluetopia.cmake)
    else()
        message("Nor bluetopia not bluekitchen - wont compile")
    endif()
else()
    message("Build with BlueKitchen")
    include(lib/btstack.cmake)
endif()


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
)

target_link_libraries(
    ${PROJECT_NAME}
    module-bsp
    module-utils
    module-vfs
    module-sys
    ${BOARD_DIR_LIBRARIES}
    )
