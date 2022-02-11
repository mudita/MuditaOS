project(module-os VERSION 1.0
        DESCRIPTION "OS module library")

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
        ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/timers.c

        ${CMAKE_CURRENT_SOURCE_DIR}/memory/usermem.c

        ${CMAKE_CURRENT_SOURCE_DIR}/CriticalSectionGuard.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/LockGuard.cpp
        )

add_library(${PROJECT_NAME} STATIC)
target_sources(module-os PRIVATE ${SOURCES})
if(NOT ${SYSTEM_VIEW_ENABLED})
        target_sources(module-os PRIVATE FreeRTOS/tasks.c)
endif()
if(${PROF_ON})
        target_sources(module-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/prof.c)
endif()

add_board_subdirectory(board)

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})

target_compile_definitions(${PROJECT_NAME}

        PUBLIC

        CPP_FREERTOS_CONDITION_VARIABLES
)

# enforce no optimization for RelWithDebInfo configuration to debug system memory
if (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        target_compile_options (${PROJECT_NAME} PRIVATE "-O0")
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_FREERTOS)
endif ()

target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}

        ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/include
        ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS
        ${CMAKE_CURRENT_SOURCE_DIR}/RTOSWrapper/include

        )

#Add FreeRTOS SystemvView headers if enabled
if((${PROJECT_TARGET} STREQUAL "TARGET_RT1051") AND (${SYSTEM_VIEW_ENABLED}))
    target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/systemviewPatch
    )
    target_link_libraries(${PROJECT_NAME} PUBLIC segger::systemview)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC log-api board-config)

if (${ENABLE_TESTS})
    add_subdirectory(test)
endif ()
