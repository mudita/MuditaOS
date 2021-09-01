# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(apps-common STATIC)

target_sources(apps-common
    PRIVATE
        Application.cpp
        AsyncTask.cpp
        AudioOperations.cpp
        AudioOperations.hpp
        CallbackStorage.cpp
        DOMResponder.cpp
        GuiTimer.cpp
        StatusBarManager.cpp
        WindowsFactory.cpp
        notifications/NotificationData.cpp
        notifications/NotificationListItem.cpp
        notifications/NotificationProvider.cpp
        notifications/NotificationsModel.cpp
        notifications/NotificationsHandler.cpp
        notifications/NotificationsConfiguration.cpp
        notifications/policies/CallNotificationPolicy.cpp
        notifications/policies/SMSNotificationPolicy.cpp
        notifications/policies/NotificationsListPolicy.cpp
        options/OptionsModel.cpp
        options/type/OptionCall.cpp
        options/type/OptionContact.cpp
        options/type/OptionSetting.cpp
        options/type/OptionSimple.cpp
        options/type/OptionWithActiveIcons.cpp
        widgets/ActiveIconFactory.cpp
        widgets/BarGraph.cpp
        widgets/BrightnessBox.cpp
        widgets/ButtonOnOff.cpp
        widgets/BellBaseLayout.cpp
        widgets/BellSideListItem.cpp
        widgets/DateWidget.cpp
        widgets/InputBox.cpp
        widgets/ModesBox.cpp
        widgets/SpinBox.cpp
        widgets/TextWithIconsWidget.cpp
        widgets/TimeSetSpinner.cpp
        widgets/AlarmSetSpinner.cpp
        widgets/TimeSetFmtSpinner.cpp
        widgets/TimeWidget.cpp
        widgets/WidgetsUtils.cpp
        widgets/ProgressTimerImpl.cpp
        windows/AppWindow.cpp
        windows/BrightnessWindow.cpp
        windows/Dialog.cpp
        windows/OptionWindow.cpp
    PUBLIC
        widgets/TimeSetFmtSpinner.hpp
)

add_subdirectory(popups)
add_subdirectory(locks)

target_link_libraries(apps-common
    PRIVATE
        date::date
        eventstore
        Microsoft.GSL::GSL
        service-antenna
        service-appmgr
        service-audio
        service-cellular
        service-evtmgr
        service-time
    PUBLIC
        module-audio
        module-bsp
        module-db
        module-gui
        module-os
        module-services
        module-sys
        module-utils
        time
        utf8
)

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(apps-common PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(apps-common PUBLIC ${PROJECT_TARGET})
target_include_directories(apps-common PUBLIC ${PROJECT_INCLUDES})

target_include_directories(apps-common

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/messages
        ${CMAKE_CURRENT_SOURCE_DIR}/windows
        ${CMAKE_CURRENT_SOURCE_DIR}/options
        ${CMAKE_CURRENT_SOURCE_DIR}/options/type
)

target_compile_options(apps-common

        PUBLIC

        -Wall

        $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
        $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>

        $<$<COMPILE_LANGUAGE:CXX>:-fno-non-call-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>

)

# Bell Hybrid related libs
add_library(bellgui)
target_sources( bellgui
   PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/options/type/OptionBellMenu.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/options/type/OptionBellMenu.hpp
)
target_include_directories( bellgui
   PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/options/type>
)

target_link_libraries( bellgui
   PRIVATE
      module-gui
)
