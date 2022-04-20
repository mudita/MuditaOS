project(module-services VERSION 1.0
        DESCRIPTION "Library with all services.")

module_is_test_entity()

add_library(${PROJECT_NAME} INTERFACE)

option(ENABLE_SERVICE_TEST "Enable service test" OFF) 

set(SERVICES 
    antenna
    appmgr
    audio
    bluetooth
    cellular
    db
    desktop
    eink
    evtmgr
    fileindexer
    gui
    time
    test
)

add_module_options( OPTION_PREFIX "SERVICE"
                    CATALOG_PREFIX "service"
                    DEFINES_LIST_NAME "ENABLED_SERVICES_DEFINES"
                    IN_MODULE_NAMES ${SERVICES})

set(ENABLED_SERVICES_DEFINES ${ENABLED_SERVICES_DEFINES} PARENT_SCOPE)

target_link_libraries(${PROJECT_NAME}
    INTERFACE
        service-gui
        service-eink
        service-appmgr
        module-bsp
        module-os
        module-sys
        module-utils
        module-gui
        module-db
        module-cellular
        module-audio
        service-antenna
        ${TARGET_LIBRARIES}
        utils-time
)

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} INTERFACE ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} INTERFACE ${PROJECT_TARGET})
target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_INCLUDES})
target_include_directories(${PROJECT_NAME} INTERFACE ${LWIP_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME}
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../source
        ${CMAKE_SOURCE_DIR}/module-bluetooth
)

target_compile_options(${PROJECT_NAME}
        INTERFACE
        -Wall
        $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
        $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-non-call-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
)
