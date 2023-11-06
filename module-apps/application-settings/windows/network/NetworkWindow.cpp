// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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
        return static_cast<app::ApplicationSettings *>(application)->getVolteState();
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
        std::list<gui::Option> optionsList;

        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_network_sim_cards"),
            [=]([[maybe_unused]] gui::Item &item) {
                application->switchWindow(gui::window::name::sim_cards, nullptr);
                return true;
            },
            nullptr,
            nullptr,
            gui::option::SettingRightItem::ArrowWhite,
            false));

        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            getVoltePrimaryLabel(),
            [this]([[maybe_unused]] gui::Item &item) {
                const auto volteState = getVolteStateFromSettingsApp(application);
                if (!volteState.permitted) {
                    return true;
                }

                const auto settingsApp = static_cast<app::ApplicationSettings *>(application);
                switch (volteState.enablement) {
                case cellular::VolteState::Enablement::Off:
                    settingsApp->sendVolteChangeRequest(true);
                    break;
                case cellular::VolteState::Enablement::On:
                    settingsApp->sendVolteChangeRequest(false);
                    break;
                default:
                    LOG_INFO("VoLTE state is unsettled, skipping request");
                    break;
                }

                return true;
            },
            [&](Item &item) {
                std::string labelText{};
                if (!item.focus) {
                    labelText = utils::translate("common_select");
                }
                else if (getVolteStateFromSettingsApp(application).permitted) {
                    labelText = utils::translate("common_switch");
                }
                navBar->setText(nav_bar::Side::Center, labelText);
                return true;
            },
            nullptr,
            getRightItemSetting(),
            false,
            UTF8(),
            true,
            getVolteSecondaryLabel()));

#if DISABLED_SETTINGS_OPTIONS == 1
        auto operatorsOn = operatorsSettings->getOperatorsOn();
        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
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
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
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

        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_network_apn_settings"),
            [=]([[maybe_unused]] gui::Item &item) {
                application->switchWindow(gui::window::name::apn_settings, nullptr);
                return true;
            },
            nullptr,
            nullptr,
            gui::option::SettingRightItem::ArrowWhite,
            false));

        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::select));

        return optionsList;
    }

    auto NetworkWindow::getVoltePrimaryLabel() -> std::string
    {
        const auto &volteState = getVolteStateFromSettingsApp(application);

        auto labelText = "<text>" + utils::translate("app_settings_network_voice_over_lte");
        if (!volteState.permitted) {
            if (volteState.enablement == cellular::VolteState::Enablement::On) {
                LOG_ERROR("VoLTE still enabled in modem despite not permitted by operator");
            }
            labelText += ": ";
            labelText += utils::translate("app_settings_network_volte_not_available");
        }
        labelText += "</text>";

        return labelText;
    }

    auto NetworkWindow::getVolteSecondaryLabel() -> std::string
    {
        const auto &volteState = getVolteStateFromSettingsApp(application);
        if (volteState.permitted && volteState.beta) {
            return "<text><b>" + utils::translate("common_experimental") + "</b><text>";
        }
        return {};
    }

    auto NetworkWindow::getRightItemSetting() -> option::SettingRightItem
    {
        const auto &volteState = getVolteStateFromSettingsApp(application);

        if (!volteState.permitted) {
            return option::SettingRightItem::Disabled;
        }

        switch (volteState.enablement) {
        case cellular::VolteState::Enablement::Off:
            return option::SettingRightItem::Off;
        case cellular::VolteState::Enablement::On:
            return option::SettingRightItem::On;
        default:
            return option::SettingRightItem::Transiting;
        }
    }
} // namespace gui
