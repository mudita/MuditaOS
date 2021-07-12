// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimCardsWindow.hpp"

#include <application-settings/ApplicationSettings.hpp>
#include <application-settings/data/PINSettingsSimData.hpp>
#include <OptionSetting.hpp>

#include <i18n/i18n.hpp>

namespace gui
{
    SimCardsWindow::SimCardsWindow(app::Application *app, app::settingsInterface::SimParams *simParams)
        : BaseSettingsWindow(app, gui::window::name::sim_cards), simParams(simParams)
    {}

    void SimCardsWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        setTitle(utils::translate("app_settings_network_sim_cards"));
        simParams->updateSim();
        BaseSettingsWindow::onBeforeShow(mode, data);
    }

    auto SimCardsWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optList;
        std::string simStr;
        auto phoneNumber = simParams->getNumber();
        auto sim         = simParams->getSim();
        switch (sim) {
        case Store::GSM::SIM::SIM1:
            simStr = utils::translate("app_settings_network_sim1");
            break;
        case Store::GSM::SIM::SIM2:
            simStr = utils::translate("app_settings_network_sim2");
            break;
        case Store::GSM::SIM::NONE:
        case Store::GSM::SIM::SIM_FAIL:
        case Store::GSM::SIM::SIM_UNKNOWN:
            simStr      = utils::translate("app_settings_network_sim_none");
            phoneNumber = {};
            break;
        }

        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_network_active_card"),
            [=](gui::Item &item) {
                if (Store::GSM::SIM::SIM1 == sim) {
                    simParams->setSim(Store::GSM::SIM::SIM2);
                }
                else {
                    simParams->setSim(Store::GSM::SIM::SIM1);
                }

                return true;
            },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::translate(style::strings::common::Switch), BottomBar::Side::CENTER);
                }
                else {
                    this->setBottomBarText(utils::translate(style::strings::common::select), BottomBar::Side::CENTER);
                }
                return true;
            },
            this,
            gui::option::SettingRightItem::Text,
            false,
            simStr + " / " + phoneNumber,
            true));

        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_network_pin_settings"),
            [=](gui::Item &item) {
                auto pinSettingsData = std::make_unique<gui::PINSettingsSimData>(simStr);
                this->application->switchWindow(gui::window::name::pin_settings, std::move(pinSettingsData));
                return true;
            },
            nullptr,
            nullptr,
            gui::option::SettingRightItem::ArrowWhite,
            false));

#if DISABLED_SETTINGS_OPTIONS == 1
        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_network_import_contacts_from_sim_card"),
            [=](gui::Item &item) {
                this->application->switchWindow(gui::window::name::import_contacts, nullptr);
                return true;
            },
            nullptr,
            nullptr));
#endif // DISABLED_SETTINGS_OPTIONS

        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));

        return optList;
    }
} // namespace gui
