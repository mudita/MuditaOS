# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(apps-common STATIC)

target_sources(apps-common
    PRIVATE
        ApplicationCommon.cpp
        ApplicationCommonPopupBlueprints.cpp
        AsyncTask.cpp
        AudioOperations.cpp
        AudioOperations.hpp
        CallbackStorage.cpp
        DOMResponder.cpp
        GuiTimer.cpp
        PopupBlueprintFactory.cpp
        StatusBarManager.cpp
        WindowsFactory.cpp
        WindowsStack.cpp
        WindowsPopupQueue.cpp
        WindowsPopupFilter.cpp
        WindowsPopupRequest.cpp

        audio/SoundsPlayer.cpp

        models/SongContext.cpp
        models/SongsRepository.cpp

        notifications/NotificationData.cpp
        notifications/NotificationListItem.cpp
        notifications/NotificationProvider.cpp
        notifications/NotificationsModel.cpp
        notifications/NotificationsListPresenter.cpp
        notifications/NotificationTilesPresenter.cpp
        notifications/NotificationsHandler.cpp
        notifications/NotificationsConfiguration.cpp
        notifications/NotificationTilesBox.cpp
        notifications/policies/CallNotificationPolicy.cpp
        notifications/policies/SMSNotificationPolicy.cpp
        notifications/policies/NotificationsListPolicy.cpp

        options/OptionsModel.cpp
        options/OptionsList.cpp
        options/type/OptionCall.cpp
        options/type/OptionContact.cpp
        options/type/OptionSetting.cpp
        options/type/OptionSimple.cpp
        options/type/OptionWithActiveIcons.cpp

        widgets/ActiveIconFactory.cpp
        widgets/BarGraph.cpp
        widgets/BrightnessBox.cpp
        widgets/ButtonTriState.cpp
        widgets/BellBaseLayout.cpp
        widgets/BellSideListItem.cpp
        widgets/ClockDateWidget.cpp
        widgets/DateSetSpinner.cpp
        widgets/DateWidget.cpp
        widgets/InputBox.cpp
        widgets/ModesBox.cpp
        widgets/SpinBox.cpp
        widgets/TextSpinnerBox.cpp
        widgets/TextSpinnerBoxWithLabel.cpp
        widgets/TextWithIconsWidget.cpp
        widgets/TimeSetSpinner.cpp
        widgets/AlarmSetSpinner.cpp
        widgets/TimeSetFmtSpinner.cpp
        widgets/TimeWidget.cpp
        widgets/TimeFixedWidget.cpp
        widgets/TimeMinuteSecondWidget.cpp
        widgets/DigitsContainer.cpp
        widgets/WidgetsUtils.cpp
        widgets/ProgressTimer.cpp
        widgets/ProgressTimerWithBarGraphAndCounter.cpp

        windows/AppWindow.cpp
        windows/BrightnessWindow.cpp
        windows/Dialog.cpp
        windows/OptionWindow.cpp

    PUBLIC
        ApplicationCommon.hpp

        actions/AlarmRingingData.hpp
        actions/AlarmTriggeredAction.hpp

        models/SongContext.hpp
        models/SongsRepository.hpp
        models/SongsModelInterface.hpp

        widgets/TimeSetFmtSpinner.hpp
        widgets/spinners/ItemSpinner.hpp
        widgets/spinners/Spinners.hpp
)

add_subdirectory(popups)
add_subdirectory(locks)

target_link_libraries(apps-common
    PRIVATE
        app
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
        utils-time
        utils-language
        utf8
        version-header
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

        $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
)
