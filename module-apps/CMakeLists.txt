# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

project(module-apps VERSION 1.0
        DESCRIPTION "Library with all applications.")

module_is_test_entity()

add_library(${PROJECT_NAME} STATIC)

add_subdirectory(apps-common)

option(ENABLE_APP_ANTENNA "Enable application antenna" OFF) 
option(ENABLE_APP_TEST "Enable test application" OFF) 

set(APPLICATIONS
    alarm-clock
    antenna
    calculator
    calendar
    call
    desktop
    meditation
    messages
    music-player
    notes
    onboarding
    phonebook
    calllog
    special-input
    settings
    test
)

add_module_options( OPTION_PREFIX "APP"
                    CATALOG_PREFIX "application"
                    DEFINES_LIST_NAME "ENABLED_APPS_DEFINES"
                    IN_MODULE_NAMES ${APPLICATIONS})

target_compile_definitions(application-desktop PRIVATE "${ENABLED_APPS_DEFINES}")
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

option(DISABLED_SETTINGS_OPTIONS "Enable Disabled Settings Options" OFF)
if(${DISABLED_SETTINGS_OPTIONS})
    target_compile_definitions(${PROJECT_NAME} PUBLIC DISABLED_SETTINGS_OPTIONS=1)
endif()

option(DEVELOPER_SETTINGS_OPTIONS "Enable Developer Settings Options" ${DEVELOPER_SETTINGS_OPTIONS_DEFAULT})
if(${DEVELOPER_SETTINGS_OPTIONS})
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEVELOPER_SETTINGS_OPTIONS=1)
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        app
        $<$<BOOL:${ENABLE_APP_CALENDAR}>:application-calendar>
        $<$<BOOL:${ENABLE_APP_MESSAGES}>:application-messages>
        $<$<BOOL:${ENABLE_APP_SETTINGS}>:application-settings>
        apps-common
        date::date
        eventstore
        i18n
        libphonenumber::libphonenumber
        Microsoft.GSL::GSL
        service-antenna
        service-appmgr
        service-audio
        service-cellular
        service-db
        service-evtmgr
        service-time-api
        utils-time
    PUBLIC
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
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})

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

        $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>

)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
