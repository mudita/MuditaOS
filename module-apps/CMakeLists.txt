# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

project(module-apps VERSION 1.0
        DESCRIPTION "Library with all applications.")

module_is_test_entity()

if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(targets/Target_RT1051.cmake)
elseif(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Linux.cmake)
else()
    message(FATAL_ERROR "Invalid target!")
endif()

set( SOURCES 
    "Application.cpp"
    "GuiTimer.cpp"
    "DOMResponder.cpp"
    "WindowsFactory.cpp"
    "StatusBarManager.cpp"
    "AsyncTask.cpp"
    "CallbackStorage.cpp"
    "windows/AppWindow.cpp"
    "windows/OptionWindow.cpp"
    "windows/Dialog.cpp"
    "windows/NoEvents.cpp"
    "widgets/ButtonOnOff.cpp"
    "widgets/InputBox.cpp"
    "windows/BrightnessWindow.cpp"
    "widgets/BrightnessBox.cpp"
    "widgets/ModesBox.cpp"
    "widgets/BarGraph.cpp"
    "widgets/ActiveIconFactory.cpp"
    "widgets/TextWithIconsWidget.cpp"
    "widgets/DateWidget.cpp"
    "widgets/IceBox.cpp"
    "widgets/TimeWidget.cpp"
    "widgets/WidgetsUtils.cpp"
    "notifications/NotificationListItem.cpp"
    "notifications/NotificationsModel.cpp"
    "notifications/NotificationProvider.cpp"
    "options/OptionsModel.cpp"
    "options/type/OptionSimple.cpp"
    "options/type/OptionCall.cpp"
    "options/type/OptionContact.cpp"
    "options/type/OptionSetting.cpp"
    "options/type/OptionWithActiveIcons.cpp"
    "notifications/NotificationData.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})

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
    antenna
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

include_directories( ../module-sys )
include_directories( ../module-utils )
include_directories( ../module-services )
include_directories( ../module-gui )
include_directories( ../module-os )
include_directories( ../module-bsp )
include_directories( ../module-db )

add_subdirectory(popups)
add_subdirectory(locks)
add_subdirectory(application-bell-main)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
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
        ${CMAKE_CURRENT_SOURCE_DIR}/messages
        ${CMAKE_CURRENT_SOURCE_DIR}/windows
        ${CMAKE_CURRENT_SOURCE_DIR}/options
        ${CMAKE_CURRENT_SOURCE_DIR}/options/type
        ${CMAKE_BINARY_DIR}
)


target_compile_definitions(${PROJECT_NAME}

        PUBLIC

        )

target_compile_options(${PROJECT_NAME}

        PUBLIC

        -Wall

        $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
        $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>

        $<$<COMPILE_LANGUAGE:CXX>:-fno-non-call-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>

)

target_compile_features(${PROJECT_NAME} PUBLIC

        cxx_std_14
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
