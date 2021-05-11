
include_directories( ${CMAKE_PROJECT_NAME}

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
)

include_directories( ${PROJECT_NAME}

    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_sources( ${PROJECT_NAME}

    PRIVATE
        ApplicationSettings.cpp
        models/ApnSettingsModel.cpp
        models/BluetoothSettingsModel.cpp
        models/NewApnModel.cpp
        models/DateAndTimeModel.cpp
        models/FromTimeToTimeModel.cpp
        models/QuotesModel.cpp
        models/CategoriesModel.cpp
        models/SARInfoRepository.cpp
        models/FactoryData.cpp
        models/AudioSettingsModel.cpp
        models/SoundsModel.cpp
        presenter/SARInfoWindowPresenter.cpp
        presenter/TechnicalWindowPresenter.cpp
        widgets/QuoteWidget.cpp
        widgets/CategoryWidget.cpp
        widgets/ApnInputWidget.cpp
        widgets/SettingsDateItem.cpp
        widgets/SettingsTimeItem.cpp
        widgets/SettingsSoundItem.cpp
        windows/SettingsMainWindow.cpp
        windows/AddDeviceWindow.cpp
        windows/AllDevicesWindow.cpp
        windows/ApnSettingsWindow.cpp
        windows/ApnOptionsWindow.cpp
        windows/BaseSettingsWindow.cpp
        windows/BluetoothWindow.cpp
        windows/FontSizeWindow.cpp
        windows/InputLanguageWindow.cpp
        windows/DisplayLightWindow.cpp
        windows/KeypadLightWindow.cpp
        windows/LockedScreenWindow.cpp
        windows/DisplayAndKeypadWindow.cpp
        windows/AppsAndToolsWindow.cpp
        windows/NetworkWindow.cpp
        windows/NightshiftWindow.cpp
        windows/OptionsWidgetMaker.cpp
        windows/PhoneWindow.cpp
        windows/MessagesWindow.cpp
        windows/CalendarWindow.cpp
        windows/AlarmClockWindow.cpp
        windows/SoundSelectWindow.cpp
        windows/PhoneNameWindow.cpp
        windows/AutolockWindow.cpp
        windows/TorchWindow.cpp
        windows/WallpaperWindow.cpp
        windows/QuotesMainWindow.cpp
        windows/QuotesAddWindow.cpp
        windows/QuotesOptionsWindow.cpp
        windows/SARInfoWindow.cpp
        windows/SecurityMainWindow.cpp
        windows/NewApnWindow.cpp
        widgets/SpinBox.cpp
        widgets/SpinBoxOptionSettings.cpp
        windows/SystemMainWindow.cpp
        windows/LanguagesWindow.cpp
        windows/DateAndTimeMainWindow.cpp
        windows/ChangeTimeZone.cpp
        windows/ChangeDateAndTimeWindow.cpp
        windows/BluetoothCheckPasskeyWindow.cpp
        windows/EditQuotesWindow.cpp
        windows/QuoteCategoriesWindow.cpp
        windows/ConnectionFrequencyWindow.cpp
        windows/DoNotDisturbWindow.cpp
        windows/OfflineWindow.cpp
        windows/PhoneModesWindow.cpp
        windows/PINSettingsWindow.cpp
        windows/AboutYourPureWindow.cpp
        windows/CertificationWindow.cpp
        windows/TechnicalInformationWindow.cpp

    PUBLIC
        ApplicationSettings.hpp
        widgets/ApnInputWidget.hpp
        widgets/CategoryWidget.hpp
        windows/NewApnWindow.hpp
        windows/SettingsMainWindow.hpp
        windows/BaseSettingsWindow.hpp
        windows/FontSizeWindow.hpp
        windows/InputLanguageWindow.hpp
        windows/DisplayLightWindow.hpp
        windows/KeypadLightWindow.hpp
        windows/LockedScreenWindow.hpp
        windows/DisplayAndKeypadWindow.hpp
        windows/AppsAndToolsWindow.hpp
        windows/MessagesWindow.hpp
        widgets/SettingsStyle.hpp
        windows/AutolockWindow.hpp
        windows/WallpaperWindow.hpp
        windows/SystemMainWindow.hpp
        windows/LanguagesWindow.hpp
        windows/DateAndTimeMainWindow.hpp
        windows/ChangeDateAndTimeWindow.hpp
        windows/EditQuotesWindow.hpp
        windows/QuoteCategoriesWindow.hpp
        windows/AboutYourPureWindow.hpp
        windows/CertificationWindow.hpp
        windows/TechnicalInformationWindow.hpp
)

add_dependencies(${PROJECT_NAME} version)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        service-bluetooth
)
