// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimPINSettingsWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>
#include <OptionSetting.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{
    SimPINSettingsWindow::SimPINSettingsWindow(app::ApplicationCommon *app,
                                               std::unique_ptr<SimPINSettingsWindowContract::Presenter> simPINPresenter)
        : BaseSettingsWindow(app, window::name::sim_pin_settings), presenter(std::move(simPINPresenter))
    {
        presenter->attach(this);
        presenter->requestLockState();
    }

    void SimPINSettingsWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        presenter->onBeforeShow(mode, data);
    }

    auto SimPINSettingsWindow::buildOptionsList() -> std::list<Option>
    {
        std::list<Option> optionList;

        optionList.emplace_back(std::make_unique<option::OptionSettings>(
            utils::translate("app_settings_network_pin"),
            [=](Item &) {
                presenter->togglePinState();
                return true;
            },
            [=](Item &item) {
                if (item.focus) {
                    this->setNavBarText(utils::translate(style::strings::common::Switch), nav_bar::Side::Center);
                }
                else {
                    this->setNavBarText(utils::translate(style::strings::common::select), nav_bar::Side::Center);
                }
                return true;
            },
            nullptr,
            presenter->isPinEnabled() ? option::SettingRightItem::On : option::SettingRightItem::Off));

        if (presenter->isPinEnabled()) {
            optionList.emplace_back(std::make_unique<option::OptionSettings>(
                utils::translate("app_settings_network_pin_change_code"),
                [=](Item &) {
                    application->getSimLockSubject().changeSimPin();
                    return true;
                },
                nullptr,
                nullptr,
                option::SettingRightItem::ArrowWhite));
        }

        return optionList;
    }

    void SimPINSettingsWindow::setNavbarCenterActive(bool state)
    {
        setNavBarActive(nav_bar::Side::Center, state);
    }

    void SimPINSettingsWindow::setTitle(const UTF8 &text)
    {
        const auto currentWindow = application->getCurrentWindow();
        currentWindow->setTitle(text);
    }

    void SimPINSettingsWindow::refreshOptionsList()
    {
        BaseSettingsWindow::refreshOptionsList();
    }
} // namespace gui
