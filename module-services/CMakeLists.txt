cmake_minimum_required(VERSION 3.14)

project(module-services VERSION 1.0
        DESCRIPTION "Library with all services.")

set(CMAKE_CXX_STANDARD 14)

message("PROJECT TARGET: ${PROJECT_TARGET}")
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
add_subdirectory( service-kbd )
add_subdirectory( service-appmgr )

target_include_directories( ${PROJECT_NAME} PUBLIC "../module-sys" )
target_include_directories( ${PROJECT_NAME} PUBLIC "../module-utils" )
target_include_directories( ${PROJECT_NAME} PUBLIC "../module-gui" )
target_include_directories( ${PROJECT_NAME} PUBLIC "../module-os" )
target_include_directories( ${PROJECT_NAME} PUBLIC "../module-bsp" )
target_include_directories( ${PROJECT_NAME} PUBLIC "../module-apps"  )

target_link_libraries(${PROJECT_NAME} PUBLIC module-bsp module-os module-sys module-utils module-gui module-apps  ${TARGET_LIBRARIES} )

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})



target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../source

)


target_compile_definitions(${PROJECT_NAME}

        PUBLIC

        CPP_FREERTOS_CONDITION_VARIABLES
        )

target_compile_options(${PROJECT_NAME}

        PUBLIC

        -Wall

        $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
        $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>

        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-non-call-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>

)

target_compile_features(${PROJECT_NAME} PUBLIC

        cxx_std_14
        cxx_noexcept
)





# Host target configuration(mainly used for unit testing)

if(DEFINED BUILD_UNIT_TESTS)

#       add_executable(unittest_${PROJECT_NAME}
#                ${CMAKE_CURRENT_LIST_DIR}/tests/Test1/TestService1.cpp
#                ${CMAKE_CURRENT_LIST_DIR}/tests/Test1/CommandsService.cpp
#                ${CMAKE_CURRENT_LIST_DIR}/tests/Test1/DelayedService.cpp
#
#                ${CMAKE_CURRENT_LIST_DIR}/tests/tests.cpp
#                ${CMAKE_CURRENT_LIST_DIR}/tests/tests-main.cpp
#                ${CMAKE_CURRENT_LIST_DIR}/tests/Common/SchedulerKiller.cpp
#        )
#
#           target_compile_definitions(${PROJECT_NAME} PUBLIC UNIT_TESTS)
#
#           target_include_directories(unittest_${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tests )
#
#           target_compile_definitions(unittest_${PROJECT_NAME} PUBLIC COMPILE_HOST)
#
#           target_compile_definitions(unittest_${PROJECT_NAME} PUBLIC LINUX_PORT_DEBUG)
#
#           target_compile_definitions(unittest_${PROJECT_NAME} PUBLIC MODULE_CORE_CUSTOM_BUS)
#
#           target_compile_options(unittest_${PROJECT_NAME} PUBLIC -fsanitize=address )
#
#
#
#           target_link_libraries(unittest_${PROJECT_NAME} PRIVATE pthread ${PROJECT_NAME})
#
#           target_link_options(unittest_${PROJECT_NAME} PUBLIC -fsanitize=address)


endif()

