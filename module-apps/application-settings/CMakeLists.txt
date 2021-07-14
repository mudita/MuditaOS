
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
        models/advanced/ColorTestModel.cpp
        models/bluetooth/BluetoothSettingsModel.cpp
        models/network/ApnSettingsModel.cpp
        models/network/NewApnModel.cpp
        models/display-keypad/QuotesModel.cpp
        models/display-keypad/CategoriesModel.cpp
        models/apps/AudioSettingsModel.cpp
        models/apps/SoundsModel.cpp
        models/DateAndTimeModel.cpp
        models/SARInfoRepository.cpp
        models/FactoryData.cpp
        presenter/SARInfoWindowPresenter.cpp
        presenter/TechnicalWindowPresenter.cpp
        widgets/advanced/ColorTestListItem.cpp
        widgets/advanced/ColorTestListView.cpp
        widgets/network/ApnInputWidget.cpp
        widgets/display-keypad/QuoteWidget.cpp
        widgets/display-keypad/CategoryWidget.cpp
        widgets/apps/SettingsSoundItem.cpp
        widgets/SettingsDateItem.cpp
        widgets/SettingsTimeItem.cpp
        widgets/SpinBoxOptionSettings.cpp
        windows/SettingsMainWindow.cpp
        windows/advanced/AdvancedOptionsWindow.cpp
        windows/advanced/InformationWindow.cpp
        windows/advanced/UITestWindow.cpp
        windows/advanced/EinkModeWindow.cpp
        windows/advanced/ColorTestWindow.cpp
        windows/bluetooth/AddDeviceWindow.cpp
        windows/bluetooth/AllDevicesWindow.cpp
        windows/bluetooth/BluetoothWindow.cpp
        windows/bluetooth/PhoneNameWindow.cpp
        windows/bluetooth/BluetoothCheckPasskeyWindow.cpp
        windows/network/NetworkWindow.cpp
        windows/network/SimCardsWindow.cpp
        windows/network/SimPINSettingsWindow.cpp
        windows/network/ApnSettingsWindow.cpp
        windows/network/ApnOptionsWindow.cpp
        windows/network/NewApnWindow.cpp
        windows/display-keypad/DisplayAndKeypadWindow.cpp
        windows/display-keypad/DisplayLightWindow.cpp
        windows/display-keypad/FontSizeWindow.cpp
        windows/display-keypad/WallpaperWindow.cpp
        windows/display-keypad/QuotesMainWindow.cpp
        windows/display-keypad/QuotesAddWindow.cpp
        windows/display-keypad/QuotesOptionsWindow.cpp
        windows/display-keypad/EditQuotesWindow.cpp
        windows/display-keypad/QuoteCategoriesWindow.cpp
        windows/display-keypad/KeypadLightWindow.cpp
        windows/display-keypad/InputLanguageWindow.cpp
        windows/phone-modes/PhoneModesWindow.cpp
        windows/phone-modes/DoNotDisturbWindow.cpp
        windows/phone-modes/OfflineWindow.cpp
        windows/phone-modes/ConnectionFrequencyWindow.cpp
        windows/apps/AppsWindow.cpp
        windows/apps/PhoneWindow.cpp
        windows/apps/MessagesWindow.cpp
        windows/apps/AlarmClockWindow.cpp
        windows/apps/SoundSelectWindow.cpp
        windows/security/SecurityMainWindow.cpp
        windows/security/AutolockWindow.cpp
        windows/BaseSettingsWindow.cpp
        windows/OptionsWidgetMaker.cpp
        windows/SARInfoWindow.cpp
        windows/SystemMainWindow.cpp
        windows/LanguagesWindow.cpp
        windows/DateAndTimeMainWindow.cpp
        windows/ChangeTimeZone.cpp
        windows/ChangeDateAndTimeWindow.cpp
        windows/AboutYourPureWindow.cpp
        windows/CertificationWindow.cpp
        windows/TechnicalInformationWindow.cpp

    PUBLIC
        ApplicationSettings.hpp
        models/advanced/ColorTestModel.hpp
        widgets/advanced/ColorTestListItem.hpp
        widgets/advanced/ColorTestListItem.hpp
        widgets/advanced/ColorTestListStyle.hpp
        widgets/network/ApnInputWidget.hpp
        widgets/display-keypad/CategoryWidget.hpp
        widgets/SettingsStyle.hpp
        windows/SettingsMainWindow.hpp
        windows/WindowNames.hpp
        windows/advanced/AdvancedOptionsWindow.hpp
        windows/advanced/InformationWindow.hpp
        windows/advanced/UITestWindow.hpp
        windows/advanced/EinkModeWindow.hpp
        windows/advanced/ColorTestWindow.hpp
        windows/network/SimCardsWindow.hpp
        windows/network/NewApnWindow.hpp
        windows/display-keypad/DisplayAndKeypadWindow.hpp
        windows/display-keypad/DisplayLightWindow.hpp
        windows/display-keypad/FontSizeWindow.hpp
        windows/display-keypad/WallpaperWindow.hpp
        windows/display-keypad/EditQuotesWindow.hpp
        windows/display-keypad/QuoteCategoriesWindow.hpp
        windows/display-keypad/KeypadLightWindow.hpp
        windows/display-keypad/InputLanguageWindow.hpp
        windows/apps/AppsWindow.hpp
        windows/apps/MessagesWindow.hpp
        windows/security/AutolockWindow.hpp
        windows/BaseSettingsWindow.hpp
        windows/SystemMainWindow.hpp
        windows/LanguagesWindow.hpp
        windows/DateAndTimeMainWindow.hpp
        windows/ChangeDateAndTimeWindow.hpp
        windows/AboutYourPureWindow.hpp
        windows/CertificationWindow.hpp
        windows/TechnicalInformationWindow.hpp
)

add_dependencies(${PROJECT_NAME} version)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        service-bluetooth
        eventstore
    PRIVATE
        service-time-api
)
