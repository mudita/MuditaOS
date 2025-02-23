module_is_test_entity(bell-applications-common)

add_library(application-bell-common STATIC)
add_library(bell::app-common ALIAS application-bell-common)

target_include_directories(application-bell-common
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/common>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-bell-common
    PRIVATE
        src/BellPowerOffPresenter.cpp
        src/BellFactoryResetPresenter.cpp
        src/AlarmModel.cpp
        src/AudioModel.cpp
        src/TimeModel.cpp
        src/UserSessionModel.cpp
        src/BatteryModel.cpp
        src/QuoteModel.cpp
        src/BatteryLevelNotificationModel.cpp
        src/SoundsRepository.cpp
        src/BellListItemProvider.cpp
        src/BellSideListItemWithCallbacks.cpp
        src/TimeUtils.cpp
        src/AudioErrorPresenter.cpp

        src/windows/BellFactoryReset.cpp
        src/windows/BellFinishedWindow.cpp
        src/windows/BellTurnOffWindow.cpp
        src/windows/BellWelcomeWindow.cpp
        src/windows/SessionPausedWindow.cpp
        src/windows/ShortcutsWindow.cpp
        src/windows/BellBatteryStatusWindow.cpp
        src/windows/AppsBatteryStatusWindow.cpp
        src/windows/AudioErrorWindow.cpp
        src/windows/UpdateInstructionWindow.cpp

        src/models/SettingsModel.cpp
        src/models/BedtimeModel.cpp
        src/models/LowBatteryInfoModel.cpp
        src/models/SongsModel.cpp
        src/models/PreWakeUpModel.cpp
        src/models/RelaxationFadeModel.cpp

        src/popups/AlarmActivatedWindow.cpp
        src/popups/AlarmActivatedWindow.cpp
        src/popups/BedtimeNotificationWindow.cpp
        src/popups/AlarmDeactivatedWindow.cpp
        src/popups/BellTurnOffOptionWindow.cpp
        src/popups/BellRebootWindow.cpp
        src/popups/ChargingNotificationWindow.cpp
        src/popups/presenter/AlarmActivatedPresenter.cpp

        src/widgets/AlarmIcon.cpp
        src/widgets/ListItems.cpp
        src/widgets/BellBattery.cpp
        src/widgets/BellConnectionStatus.cpp
        src/widgets/BellStatusClock.cpp
        src/widgets/DuoHBox.cpp
        src/widgets/ProgressTimerWithSnoozeTimer.cpp
        src/widgets/SnoozeTimer.cpp
        src/widgets/LayoutVertical.cpp
        src/widgets/ClockVertical.cpp
        src/widgets/ListViewWithLabels.cpp
        src/widgets/LabelMarkerItem.cpp
        src/widgets/LabelOptionWithTick.cpp

        src/options/BellOptionWindow.cpp
        src/options/BellShortOptionWindow.cpp
        src/options/BellOptionWithDescriptionWindow.cpp
        src/options/OptionBellMenu.cpp
        src/options/BellOptionsNavigation.cpp

        src/layouts/HomeScreenLayouts.cpp
        src/layouts/HomeScreenLayoutClassic.cpp
        src/layouts/HomeScreenLayoutClassicWithAmPm.cpp
        src/layouts/HomeScreenLayoutClassicWithBattery.cpp
        src/layouts/HomeScreenLayoutClassicWithDate.cpp
        src/layouts/HomeScreenLayoutClassicWithQuotes.cpp
        src/layouts/HomeScreenLayoutClassicWithQuotesAmPm.cpp
        src/layouts/HomeScreenLayoutClassicWithTemp.cpp
        src/layouts/HomeScreenLayoutVertical.cpp
        src/layouts/HomeScreenLayoutVerticalSimple.cpp
        src/layouts/HomeScreenLayoutVerticalWithAmPm.cpp
        src/layouts/HomeScreenLayoutVerticalWithDate.cpp
        src/layouts/ShortcutsLayouts.cpp
        src/layouts/ShortcutsLayoutClassic.cpp
        src/layouts/UpdateInstructionLayouts.cpp
        src/layouts/UpdateInstructionLayoutClassic.cpp

    PUBLIC
        include/common/BellListItemProvider.hpp
        include/common/SoundsRepository.hpp
        include/common/BellPowerOffPresenter.hpp
        include/common/BellFactoryResetPresenter.hpp
        include/common/AudioErrorPresenter.hpp
        include/common/windows/BellFactoryReset.hpp
        include/common/windows/BellFinishedWindow.hpp
        include/common/windows/BellTurnOffWindow.hpp
        include/common/windows/BellWelcomeWindow.hpp
        include/common/windows/SessionPausedWindow.hpp
        include/common/windows/ShortcutsWindowContract.hpp
        include/common/windows/ShortcutsWindow.hpp
        include/common/windows/BellBatteryStatusWindow.hpp
        include/common/windows/AppsBatteryStatusWindow.hpp
        include/common/windows/AudioErrorWindow.hpp
        include/common/windows/UpdateInstructionWindowContract.hpp
        include/common/windows/UpdateInstructionWindow.hpp
        include/common/TimeUtils.hpp

        include/common/data/BatteryUtils.hpp
        include/common/data/BatteryStatusSwitchData.hpp

        include/common/models/AbstractAlarmModel.hpp
        include/common/models/AbstractRelaxationFadeModel.hpp
        include/common/models/AbstractBedtimeModel.hpp
        include/common/models/AbstractSettingsModel.hpp
        include/common/models/AbstractAudioModel.hpp
        include/common/models/AlarmModel.hpp
        include/common/models/SettingsModel.hpp
        include/common/models/BedtimeModel.hpp
        include/common/models/BatteryModel.hpp
        include/common/models/RelaxationFadeModel.hpp
        include/common/models/BatteryLevelNotificationModel.hpp
        include/common/models/TimeModel.hpp
        include/common/models/UserSessionModel.hpp
        include/common/models/AudioModel.hpp
        include/common/models/FrontlightModel.hpp
        include/common/models/LayoutModel.hpp
        include/common/models/LowBatteryInfoModel.hpp
        include/common/models/QuoteModel.hpp
        include/common/models/SongsModel.hpp
        include/common/models/PreWakeUpModel.hpp

        include/common/popups/presenter/AlarmActivatedPresenter.hpp
        include/common/popups/AlarmActivatedWindow.hpp
        include/common/popups/AlarmDeactivatedWindow.hpp
        include/common/popups/BedtimeNotificationWindow.hpp
        include/common/popups/BellTurnOffOptionWindow.hpp
        include/common/popups/BellRebootWindow.hpp
        include/common/popups/ChargingNotificationWindow.hpp

        include/common/widgets/AlarmIcon.hpp
        include/common/widgets/BellBattery.hpp
        include/common/widgets/BellConnectionStatus.hpp
        include/common/widgets/BellSideListItemWithCallbacks.hpp
        include/common/widgets/BellStatusClock.hpp
        include/common/widgets/DuoHBox.hpp
        include/common/widgets/ProgressTimerWithSnoozeTimer.hpp
        include/common/widgets/SnoozeTimer.hpp
        include/common/widgets/ListItems.hpp
        include/common/widgets/LayoutVertical.hpp
        include/common/widgets/ClockVertical.hpp
        include/common/widgets/ListViewWithLabels.hpp
        include/common/widgets/LabelMarkerItem.hpp
        include/common/widgets/LabelOptionWithTick.hpp

        include/common/options/BellOptionWindow.hpp
        include/common/options/BellShortOptionWindow.hpp
        include/common/options/OptionBellMenu.hpp

        include/common/layouts/HomeScreenLayouts.hpp
        include/common/layouts/BaseHomeScreenLayoutProvider.hpp
        include/common/layouts/HomeScreenLayoutClassic.hpp
        include/common/layouts/HomeScreenLayoutClassicWithAmPm.hpp
        include/common/layouts/HomeScreenLayoutClassicWithBattery.hpp
        include/common/layouts/HomeScreenLayoutClassicWithDate.hpp
        include/common/layouts/HomeScreenLayoutClassicWithQuotes.hpp
        include/common/layouts/HomeScreenLayoutClassicWithQuotesAmPm.hpp
        include/common/layouts/HomeScreenLayoutClassicWithTemp.hpp
        include/common/layouts/HomeScreenLayoutVertical.hpp
        include/common/layouts/HomeScreenLayoutVerticalSimple.hpp
        include/common/layouts/HomeScreenLayoutVerticalWithAmPm.hpp
        include/common/layouts/HomeScreenLayoutVerticalWithDate.hpp
        include/common/layouts/HomeScreenLayoutNames.hpp
        include/common/layouts/ShortcutsLayouts.hpp
        include/common/layouts/ShortcutsLayoutProvider.hpp
        include/common/layouts/ShortcutsLayoutClassic.hpp
        include/common/layouts/UpdateInstructionLayouts.hpp
        include/common/layouts/UpdateInstructionLayoutProvider.hpp
        include/common/layouts/UpdateInstructionLayoutClassic.hpp
)

target_link_libraries(application-bell-common
    PUBLIC
        apps-common
        service-db
        service-time
        utils-language

    PRIVATE
        bell::app-main
        bell::app-alarm
        bell::audio
        bell::db
        module-gui
        Microsoft.GSL::GSL
)
