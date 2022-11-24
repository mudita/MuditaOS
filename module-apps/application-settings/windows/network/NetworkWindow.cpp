// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NetworkWindow.hpp"

#include <application-settings/ApplicationSettings.hpp>
#include <application-settings/windows/WindowNames.hpp>
#include <OptionSetting.hpp>
#include <widgets/ButtonTriState.hpp>

namespace
{
    cellular::VolteState getVolteStateFromSettingsApp(app::ApplicationCommon *application)
    {
        auto const *settingsApp = static_cast<app::ApplicationSettings *>(application);
        return settingsApp->getVolteState();
    }
} // namespace

namespace gui
{
    NetworkWindow::NetworkWindow(app::ApplicationCommon *app,
                                 app::settingsInterface::OperatorsSettings *operatorsSettings)
        : BaseSettingsWindow(app, gui::window::name::network), operatorsSettings(operatorsSettings)
    {
        setTitle(utils::translate("app_settings_net"));
    }

    void NetworkWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        BaseSettingsWindow::onBeforeShow(mode, data);
    }

    auto NetworkWindow::buildOptionsList() -> std::list<gui::Option>
    {
        using namespace cellular;

        std::list<gui::Option> optList;

        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_network_sim_cards"),
            [=](gui::Item &item) {
                this->application->switchWindow(gui::window::name::sim_cards, nullptr);
                return true;
            },
            nullptr,
            nullptr,
            gui::option::SettingRightItem::ArrowWhite,
            false));

        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            [this]() {
                auto ret              = utils::translate("app_settings_network_voice_over_lte");
                const auto volteState = getVolteStateFromSettingsApp(application);
                if (!volteState.permitted) {
                    if (volteState.enablement == VolteState::Enablement::On) {
                        LOG_ERROR("[VoLTE] still enabled in modem despite not permitted by operator");
                    }
                    ret += ": ";
                    ret += utils::translate("app_settings_network_volte_not_available");
                }
                return ret;
            }(),
            [this](gui::Item &item) {
                const auto volteState = getVolteStateFromSettingsApp(application);
                if (!volteState.permitted) {
                    return true;
                }
                auto *settingsApp = static_cast<app::ApplicationSettings *>(application);
                switch (volteState.enablement) {
                case VolteState::Enablement::Off:
                    settingsApp->sendVolteChangeRequest(true);
                    break;
                case VolteState::Enablement::On:
                    settingsApp->sendVolteChangeRequest(false);
                    break;
                default:
                    LOG_INFO("Skip request due unsettled VoLTE state");
                    break;
                }
                return true;
            },
            [&](Item &item) {
                navBar->setText(nav_bar::Side::Center, [&]() {
                    if (!item.focus) {
                        return utils::translate("common_select");
                    }
                    return getVolteStateFromSettingsApp(application).permitted ? utils::translate("common_switch") : "";
                }());
                return true;
            },
            nullptr,
            [&]() {
                const auto volteState = getVolteStateFromSettingsApp(application);
                if (!volteState.permitted) {
                    return option::SettingRightItem::Disabled;
                }
                switch (volteState.enablement) {
                case VolteState::Enablement::Off:
                    return option::SettingRightItem::Off;
                case VolteState::Enablement::On:
                    return option::SettingRightItem::On;
                default:
                    return option::SettingRightItem::Transiting;
                }
            }()));

#if DISABLED_SETTINGS_OPTIONS == 1
        auto operatorsOn = operatorsSettings->getOperatorsOn();
        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_network_operator_auto_select"),
            [=](gui::Item &item) {
                operatorsSettings->setOperatorsOn(!operatorsOn);
                refreshOptionsList();
                return true;
            },
            nullptr,
            nullptr,
            operatorsOn ? gui::option::SettingRightItem::On : gui::option::SettingRightItem::Off));
        if (!operatorsOn) {
            optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                utils::translate("app_settings_network_all_operators"),
                [=](gui::Item &item) {
                    this->application->switchWindow(gui::window::name::all_operators, nullptr);
                    return true;
                },
                nullptr,
                nullptr,
                gui::option::SettingRightItem::ArrowWhite,
                false));
        }
#endif // DISABLED_SETTINGS_OPTIONS

        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_network_apn_settings"),
            [=](gui::Item &item) {
                this->application->switchWindow(gui::window::name::apn_settings, nullptr);
                return true;
            },
            nullptr,
            nullptr,
            gui::option::SettingRightItem::ArrowWhite,
            false));

        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::select));

        return optList;
    }
} // namespace gui
