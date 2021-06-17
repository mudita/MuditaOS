# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

project(module-apps VERSION 1.0
        DESCRIPTION "Library with all applications.")

module_is_test_entity()

add_library(${PROJECT_NAME} STATIC)

add_subdirectory(apps-common)
add_subdirectory(application-antenna)

set(APPLICATIONS
    clock
    calendar
    desktop
    settings
    settings-new
    call
    notes
    onboarding
    phonebook
    messages
    calllog
    special-input
    music-player
    meditation
    calculator
    alarm-clock
)

list(SORT APPLICATIONS)
foreach(app IN LISTS APPLICATIONS)
    string(TOUPPER ${app} app_upper)
    string(REPLACE "-" "_" app_macro_name ${app_upper})
    option(ENABLE_APP_${app_macro_name} "Enable application ${app}" ON)
    
    if(ENABLE_APP_${app_macro_name})
        message("Enabling application: ${app}")
        add_subdirectory( application-${app} )
        list(APPEND ENABLED_APPS_DEFINES "ENABLE_APP_${app_macro_name}")
    endif()
endforeach()
set(ENABLED_APPS_DEFINES ${ENABLED_APPS_DEFINES} PARENT_SCOPE)

if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    set(OPTIMIZE_APPS_DEBUG_DEFAULT OFF)
else()
    set(OPTIMIZE_APPS_DEBUG_DEFAULT ON)
endif()
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    foreach(app IN LISTS APPLICATIONS)
        string(TOUPPER ${app} app_upper)
        string(REPLACE "-" "_" app_macro_name ${app_upper})
        option(OPTIMIZE_APP_${app_macro_name} "Optimize application in debug" ${OPTIMIZE_APPS_DEBUG_DEFAULT})
        if (${OPTIMIZE_APP_${app_macro_name}} AND ${ENABLE_APP_${app_macro_name}})
            get_target_property(APP_SOURCES ${PROJECT_NAME} SOURCES)
            list(FILTER APP_SOURCES INCLUDE REGEX "/application-${app}/")
            set_source_files_properties(${APP_SOURCES} PROPERTIES COMPILE_FLAGS "-Os")
        endif ()
    endforeach()
endif()

option(ENABLE_VOLTE "Enable VoLTE" OFF)
if(${ENABLE_VOLTE})
    target_compile_definitions(${PROJECT_NAME} PUBLIC ENABLE_VOLTE=1)
endif()

add_subdirectory(application-bell-main)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        apps-common
        date::date
        service-antenna
        service-appmgr
        service-audio
        service-cellular
        service-db
        service-evtmgr
        Microsoft.GSL::GSL
        eventstore
        time
    PUBLIC
        application-bell-main
        module-audio
        module-bsp
        module-db
        module-gui
        module-os
        module-services
        module-sys
        module-utils
        utf8
        ${TARGET_LIBRARIES}
)

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})

target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
)

target_compile_options(${PROJECT_NAME}

        PUBLIC

        -Wall

        $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
        $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>

        $<$<COMPILE_LANGUAGE:CXX>:-fno-non-call-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>

)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
