
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
        widgets/ChangePasscodeLockHandler.cpp
        widgets/QuoteWidget.cpp
        widgets/CategoryWidget.cpp
        widgets/ApnInputWidget.cpp
        widgets/SettingsDateItem.cpp
        widgets/SettingsTimeItem.cpp
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
        windows/MessagesWindow.cpp
        windows/PhoneNameWindow.cpp
        windows/AutolockWindow.cpp
        windows/TorchWindow.cpp
        windows/WallpaperWindow.cpp
        windows/QuotesMainWindow.cpp
        windows/QuotesAddWindow.cpp
        windows/QuotesOptionsWindow.cpp
        windows/SecurityMainWindow.cpp
        windows/ChangePasscodeWindow.cpp
        windows/NewApnWindow.cpp
        widgets/SpinBox.cpp
        widgets/SpinBoxOptionSetting.cpp
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
        windows/AboutYourPureWindow.cpp
        windows/CertificationWindow.cpp

    PUBLIC
        ApplicationSettings.hpp
        widgets/ChangePasscodeLockHandler.hpp
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
        windows/ChangePasscodeWindow.hpp
        windows/SystemMainWindow.hpp
        windows/LanguagesWindow.hpp
        windows/DateAndTimeMainWindow.hpp
        windows/ChangeDateAndTimeWindow.hpp
        windows/EditQuotesWindow.hpp
        windows/QuoteCategoriesWindow.hpp
        windows/AboutYourPureWindow.hpp
        windows/CertificationWindow.hpp
)

add_dependencies(${PROJECT_NAME} version)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        service-bluetooth
)
