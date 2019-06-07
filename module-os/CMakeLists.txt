cmake_minimum_required(VERSION 3.12)

project(module-os VERSION 1.0
        DESCRIPTION "OS module library")

set(CMAKE_CXX_STANDARD 14)


if(NOT DEFINED PROJECT_LIB_DIRECTORY )
    set(PROJECT_LIB_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/" CACHE STRING "Output path for static libraries")
    message("Setting PROJECT_LIB_DIRECTORY to ${PROJECT_LIB_DIRECTORY}")
endif()

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_LIB_DIRECTORY})

if(NOT DEFINED PROJECT_BUILD_DIRECTORY )
    set(PROJECT_BUILD_DIRECTORY "${CMAKE_SOURCE_DIR}/build" CACHE STRING "Output directory for building" )
    message("Setting PROJECT_BUILD_DIRECTORY to ${PROJECT_BUILD_DIRECTORY}")
endif()

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BUILD_DIRECTORY})

set(SOURCES

        ${CMAKE_CURRENT_SOURCE_DIR}/RTOSWrapper/ccondition_variable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RTOSWrapper/cevent_groups.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RTOSWrapper/cmem_pool.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RTOSWrapper/cmutex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RTOSWrapper/cqueue.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RTOSWrapper/cread_write_lock.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RTOSWrapper/csemaphore.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RTOSWrapper/ctasklet.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RTOSWrapper/cthread.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RTOSWrapper/ctickhook.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RTOSWrapper/ctimer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RTOSWrapper/cworkqueue.cpp


        ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/application.c
        ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/event_groups.c
        ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/heap_4.c
        ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/list.c
        ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/queue.c
        ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/stream_buffer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/tasks.c
        ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/timers.c

        ${CMAKE_CURRENT_SOURCE_DIR}/memory/usermem.c

        )

if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(targets/Target_RT1051.cmake)
elseif(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Linux.cmake)
else()
    message(FATAL_ERROR "Invalid target!")
endif()


add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})


# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})

target_compile_definitions(${PROJECT_NAME}

        PUBLIC

        CPP_FREERTOS_CONDITION_VARIABLES
        CPP_FREERTOS_NO_EXCEPTIONS
)

target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}

        ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/include
        ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS
        ${CMAKE_CURRENT_SOURCE_DIR}/RTOSWrapper/include

)

target_link_libraries(${PROJECT_NAME} module-bsp)

# Host target configuration(mainly used for unit testing)

if(DEFINED BUILD_UNIT_TESTS)

    target_compile_definitions(${PROJECT_NAME} PUBLIC LINUX_PORT_DEBUG)
endif()

