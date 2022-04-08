project(module-os VERSION 1.0
        DESCRIPTION "OS module library")

add_library(${PROJECT_NAME} STATIC

        RTOSWrapper/ccondition_variable.cpp
        RTOSWrapper/cevent_groups.cpp
        RTOSWrapper/cmem_pool.cpp
        RTOSWrapper/cmutex.cpp
        RTOSWrapper/cqueue.cpp
        RTOSWrapper/cread_write_lock.cpp
        RTOSWrapper/csemaphore.cpp
        RTOSWrapper/ctasklet.cpp
        RTOSWrapper/cthread.cpp
        RTOSWrapper/ctickhook.cpp
        RTOSWrapper/ctimer.cpp
        RTOSWrapper/cworkqueue.cpp

        memory/usermem.c

        LockGuard.cpp
        CriticalSectionGuard.cpp

        $<$<BOOL:${PROF_ON}>:prof/prof.c>
        )

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
    set_source_files_properties(memory/usermem.c PROPERTIES COMPILE_OPTIONS "-O0")
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG_FREERTOS)
endif ()

target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}

        RTOSWrapper/include
        prof

        )

#Add FreeRTOS SystemvView headers if enabled
if ((${PROJECT_TARGET} STREQUAL "TARGET_RT1051") AND (${SYSTEM_VIEW_ENABLED}))
    target_link_libraries(${PROJECT_NAME} PUBLIC segger::systemview)
endif ()

target_link_libraries(${PROJECT_NAME} PUBLIC log-api board-config freertos_kernel)


add_library(freertos-app application.c)
target_link_libraries(freertos-app PRIVATE freertos_kernel log-api)

if (${ENABLE_TESTS})
    add_subdirectory(test)
endif ()

