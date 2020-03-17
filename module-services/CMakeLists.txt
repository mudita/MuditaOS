cmake_minimum_required(VERSION 3.14)

project(module-services VERSION 1.0
        DESCRIPTION "Library with all services.")

include(${CMAKE_SOURCE_DIR}/module-lwip/lwip-includes.cmake)

if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(targets/Target_RT1051.cmake)
elseif(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
	set(CMAKE_CXX_STANDARD 17)
    include(targets/Target_Linux.cmake)
else()
    message(FATAL_ERROR "Invalid target!")
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})

add_subdirectory( service-gui )
add_subdirectory( service-eink )
add_subdirectory( service-evtmgr )
add_subdirectory( service-appmgr )
add_subdirectory( service-db )
add_subdirectory( service-cellular )
add_subdirectory( service-audio )
add_subdirectory( service-bluetooth )
add_subdirectory( service-lwip )
add_subdirectory( service-desktop )


target_link_libraries(${PROJECT_NAME} PUBLIC module-bsp module-os module-sys module-utils module-gui module-apps module-db module-cellular module-audio  ${TARGET_LIBRARIES} )

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${LWIP_INCLUDE_DIRS})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})

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
