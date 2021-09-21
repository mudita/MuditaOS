project(module-services VERSION 1.0
        DESCRIPTION "Library with all services.")

module_is_test_entity()

add_library(${PROJECT_NAME} STATIC ${SOURCES})

add_subdirectory( service-antenna )
add_subdirectory( service-appmgr )
add_subdirectory( service-audio )
add_subdirectory( service-bluetooth )
add_subdirectory( service-cellular )
add_subdirectory( service-db )
add_subdirectory( service-desktop )
add_subdirectory( service-eink )
add_subdirectory( service-evtmgr )
add_subdirectory( service-gui )
add_subdirectory( service-time )

target_link_libraries(${PROJECT_NAME} 
    PUBLIC
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
    PRIVATE
        utils-time
)

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${LWIP_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../source
        ${CMAKE_SOURCE_DIR}/module-bluetooth
)

target_compile_options(${PROJECT_NAME}
        PUBLIC
        -Wall
        $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
        $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-non-call-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
)
