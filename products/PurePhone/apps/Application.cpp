// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Application.hpp>
#include <popups/AlarmPopup.hpp>
#include <popups/VolumeWindow.hpp>
#include <popups/HomeModesWindow.hpp>
#include <popups/TetheringPhoneModePopup.hpp>
#include <popups/TetheringConfirmationPopup.hpp>
#include <popups/BluetoothAuthenticatePopup.hpp>
#include <popups/BluetoothInfoPopup.hpp>
#include <popups/PowerOffWindow.hpp>
#include <popups/presenter/PowerOffPresenter.hpp>
#include <popups/presenter/WallpaperPresenter.hpp>
#include <popups/lock-popups/PhoneLockedWindow.hpp>
#include <popups/lock-popups/PhoneLockedInfoWindow.hpp>
#include <popups/lock-popups/PhoneLockInputWindow.hpp>
#include <popups/lock-popups/PhoneLockChangeInfoWindow.hpp>
#include <popups/lock-popups/SimLockInputWindow.hpp>
#include <popups/lock-popups/SimInfoWindow.hpp>
#include <popups/lock-popups/SimNotReadyWindow.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

namespace app
{
    void Application::updateStatuses(gui::AppWindow *window) const
    {
        window->updateBatteryStatus();
        window->updateBluetooth(statusIndicators.bluetoothMode);
        window->updateAlarmClock(statusIndicators.alarmClockStatus);
        window->updateSim();
        window->updateSignalStrength();
        window->updateNetworkAccessTechnology();
        window->updatePhoneMode(statusIndicators.phoneMode);
        window->updateTethering(statusIndicators.tetheringState);
    }

    void Application::attachPopups(const std::vector<gui::popup::ID> &popupsList)
    {
        using namespace gui::popup;
        for (auto popup : popupsList) {
            switch (popup) {
            case ID::Volume:
                windowsFactory.attach(window::volume_window, [](ApplicationCommon *app, const std::string &name) {
                    return std::make_unique<gui::VolumeWindow>(app, window::volume_window);
                });
                break;
            case ID::Tethering:
            case ID::TetheringPhoneModeChangeProhibited:
                windowsFactory.attach(window::tethering_confirmation_window,
                                      [](ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::TetheringConfirmationPopup>(
                                              app, window::tethering_confirmation_window);
                                      });
                windowsFactory.attach(window::tethering_phonemode_change_window,
                                      [](ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::TetheringPhoneModePopup>(
                                              app, window::tethering_phonemode_change_window);
                                      });
                break;
            case ID::PhoneModes:
                windowsFactory.attach(window::phone_modes_window, [](ApplicationCommon *app, const std::string &name) {
                    return std::make_unique<gui::HomeModesWindow>(app, window::phone_modes_window);
                });
                break;
            case ID::BluetoothAuthenticate:
            case ID::BluetoothInfo:
                windowsFactory.attach(
                    window::bluetooth_authenticate, [](ApplicationCommon *app, const std::string &name) {
                        return std::make_unique<gui::BluetoothAuthenticatePopup>(app, window::bluetooth_authenticate);
                    });
                windowsFactory.attach(window::bluetooth_info, [](ApplicationCommon *app, const std::string &name) {
                    return std::make_unique<gui::BluetoothInfoPopup>(app, window::bluetooth_info);
                });
                break;
            case ID::Brightness:
                break;
            case ID::PhoneLock:
            case ID::PhoneLockInput:
            case ID::PhoneLockInfo:
            case ID::PhoneLockChangeInfo:
                windowsFactory.attach(
                    window::phone_lock_window, [this](ApplicationCommon *app, const std::string &name) {
                        auto presenter                 = std::make_unique<gui::WallpaperPresenter>(app);
                        auto lockScreenDeepRefreshRate = utils::getNumericValue<unsigned>(settings->getValue(
                            settings::Display::lockScreenDeepRefreshRate, settings::SettingsScope::Global));
                        return std::make_unique<gui::PhoneLockedWindow>(
                            app, window::phone_lock_window, std::move(presenter), lockScreenDeepRefreshRate);
                    });
                windowsFactory.attach(
                    window::phone_lock_info_window, [](ApplicationCommon *app, const std::string &name) {
                        return std::make_unique<gui::PhoneLockedInfoWindow>(app, window::phone_lock_info_window);
                    });
                windowsFactory.attach(
                    window::phone_lock_input_window, [](ApplicationCommon *app, const std::string &name) {
                        return std::make_unique<gui::PhoneLockInputWindow>(app, window::phone_lock_input_window);
                    });
                windowsFactory.attach(window::phone_lock_change_info_window,
                                      [](ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::PhoneLockChangeInfoWindow>(
                                              app, window::phone_lock_change_info_window);
                                      });
                windowsFactory.attach(window::power_off_window, [](ApplicationCommon *app, const std::string &name) {
                    auto presenter = std::make_unique<gui::PowerOffPresenter>(app);
                    return std::make_unique<gui::PowerOffWindow>(app, std::move(presenter));
                });
                break;
            case ID::SimLock:
            case ID::SimInfo:
            case ID::SimNotReady:
                windowsFactory.attach(window::sim_unlock_window, [](ApplicationCommon *app, const std::string &name) {
                    return std::make_unique<gui::SimLockInputWindow>(app, window::sim_unlock_window);
                });
                windowsFactory.attach(window::sim_info_window, [](ApplicationCommon *app, const std::string &name) {
                    return std::make_unique<gui::SimInfoWindow>(app, window::sim_info_window);
                });
                windowsFactory.attach(
                    window::sim_not_ready_window, [](ApplicationCommon *app, const std::string &name) {
                        return std::make_unique<gui::SimNotReadyWindow>(app, window::sim_not_ready_window);
                    });
                break;
            case ID::Alarm:
                windowsFactory.attach(window::alarm_window, [](ApplicationCommon *app, const std::string &name) {
                    auto presenter = std::make_shared<popup::AlarmPopupPresenter>(app);
                    return std::make_unique<gui::AlarmPopup>(app, window::alarm_window, presenter);
                });
            default:
                break;
            }
        }
    }
} // namespace app
