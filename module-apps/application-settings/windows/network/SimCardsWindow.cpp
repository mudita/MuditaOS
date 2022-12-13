// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimCardsWindow.hpp"

#include <application-settings/data/PINSettingsSimData.hpp>
#include <application-settings/windows/WindowNames.hpp>
#include <OptionSetting.hpp>

namespace gui
{
    SimCardsWindow::SimCardsWindow(app::ApplicationCommon *app, app::settingsInterface::SimParams *simParams)
        : BaseSettingsWindow(app, gui::window::name::sim_cards), simParams(simParams)
    {}

    void SimCardsWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        setTitle(utils::translate("app_settings_network_sim_cards"));
        simParams->updateSimParam();
        BaseSettingsWindow::onBeforeShow(mode, data);
    }

    auto SimCardsWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optList;
        std::string simStr;
        auto phoneNumber = simParams->getNumber();
        auto selectedSim = simParams->getSelectedSim();
        auto simState    = simParams->getSimState();

        switch (selectedSim) {
        case Store::GSM::SelectedSIM::SIM1:
            simStr = utils::translate("app_settings_network_sim1");
            break;
        case Store::GSM::SelectedSIM::SIM2:
            simStr = utils::translate("app_settings_network_sim2");
            break;
        case Store::GSM::SelectedSIM::NONE:
            simStr      = utils::translate("app_settings_network_sim_none");
            phoneNumber = {};
            break;
        }

        if (selectedSim == Store::GSM::SelectedSIM::NONE) {
            optList = createListWithNoSelectedSim(selectedSim, simStr);
        }
        else {
            switch (simState) {
            case Store::GSM::SIM::SIM1:
            case Store::GSM::SIM::SIM2:
                optList = createListWithActiveSim(selectedSim, simStr, phoneNumber);
                break;
            case Store::GSM::SIM::SIM_NEED_PIN:
            case Store::GSM::SIM::SIM_NEED_PUK:
            case Store::GSM::SIM::SIM_LOCKED:
                optList = createListWithBlockedSim(selectedSim, simStr);
                break;
            case Store::GSM::SIM::NONE:
            case Store::GSM::SIM::SIM_FAIL:
            case Store::GSM::SIM::SIM_UNKNOWN:
                optList = createListWithNoConnectedSim(selectedSim, simStr);
                break;
            }
        }

        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::select));

        return optList;
    }

    std::list<gui::Option> SimCardsWindow::createListWithNoConnectedSim(const Store::GSM::SelectedSIM sim,
                                                                        const std::string &simStr)
    {
        std::list<gui::Option> optList;
        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_network_active_card") + " (" +
                utils::translate("app_settings_network_not_connected") + ")",
            [=](gui::Item &item) { return createSwitchSimAction(sim); },
            [=](gui::Item &item) { return createNavBarText(item); },
            this,
            gui::option::SettingRightItem::Text,
            false,
            simStr,
            true));

        return optList;
    }

    std::list<gui::Option> SimCardsWindow::createListWithBlockedSim(const Store::GSM::SelectedSIM sim,
                                                                    const std::string &simStr)
    {
        std::list<gui::Option> optList;
        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_network_active_card"),
            [=](gui::Item &item) { return createSwitchSimAction(sim); },
            [=](gui::Item &item) { return createNavBarText(item); },
            this,
            gui::option::SettingRightItem::Text,
            false,
            simStr,
            true));

        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_network_unblock_card"),
            [=](gui::Item &item) {
                application->getSimLockSubject().unblockSim();
                return true;
            },
            nullptr,
            nullptr,
            gui::option::SettingRightItem::ArrowWhite));

        return optList;
    }

    std::list<gui::Option> SimCardsWindow::createListWithActiveSim(const Store::GSM::SelectedSIM sim,
                                                                   const std::string &simStr,
                                                                   const std::string &phoneNumber)
    {
        std::list<gui::Option> optList;
        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_network_active_card"),
            [=](gui::Item &item) { return createSwitchSimAction(sim); },
            [=](gui::Item &item) { return createNavBarText(item); },
            this,
            gui::option::SettingRightItem::Text,
            false,
            simStr + " / " + phoneNumber,
            true));

        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_network_pin_settings"),
            [=](gui::Item &item) {
                auto pinSettingsData = std::make_unique<gui::PINSettingsSimData>(simStr);
                this->application->switchWindow(gui::window::name::sim_pin_settings, std::move(pinSettingsData));
                return true;
            },
            nullptr,
            nullptr,
            gui::option::SettingRightItem::ArrowWhite,
            false));

        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_network_import_contacts_from_sim_card"),
            [=](gui::Item &item) {
                application->switchWindow(gui::window::name::import_contacts);
                return true;
            },
            nullptr,
            nullptr,
            gui::option::SettingRightItem::ArrowWhite));

        return optList;
    }

    std::list<gui::Option> SimCardsWindow::createListWithNoSelectedSim(const Store::GSM::SelectedSIM sim,
                                                                       const std::string &simStr)
    {
        std::list<gui::Option> optList;
        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_network_active_card"),
            [=](gui::Item &item) { return createSwitchSimAction(sim); },
            [=](gui::Item &item) { return createNavBarText(item); },
            this,
            gui::option::SettingRightItem::Text,
            false,
            simStr,
            true));

        return optList;
    }

    bool SimCardsWindow::createSwitchSimAction(const Store::GSM::SelectedSIM selectedSim) const
    {
        selectedSim == Store::GSM::SelectedSIM::SIM1 ? simParams->setSelectedSim(Store::GSM::SelectedSIM::SIM2)
                                                     : simParams->setSelectedSim(Store::GSM::SelectedSIM::SIM1);
        this->application->switchWindow(gui::popup::window::sim_switching_window);
        return true;
    }

    bool SimCardsWindow::createNavBarText(const gui::Item &item)
    {
        item.focus ? this->setNavBarText(utils::translate(style::strings::common::Switch), nav_bar::Side::Center)
                   : this->setNavBarText(utils::translate(style::strings::common::select), nav_bar::Side::Center);
        return true;
    }
} // namespace gui
