// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApnSettingsWindow.hpp"

#include <application-settings/data/ApnListData.hpp>
#include <application-settings/data/SettingsItemData.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>
#include <header/AddElementAction.hpp>

namespace gui
{

    ApnSettingsWindow::ApnSettingsWindow(app::Application *app) : OptionWindow(app, gui::window::name::apn_settings)
    {
        buildInterface();
    }

    void ApnSettingsWindow::buildInterface()
    {
        setTitle(utils::translate("app_settings_network_apn_settings"));
        header->navigationIndicatorAdd(new gui::header::AddElementAction(), gui::header::BoxSelection::Left);

        emptyListIcon = new Icon(this,
                                 0,
                                 style::window::default_vertical_pos,
                                 style::window_width,
                                 style::window_height - style::window::default_vertical_pos - style::footer::height,
                                 "empty_list_add_W_G",
                                 utils::translate("app_settings_apn_settings_no_apns"));

        bottomBar->setText(BottomBar::Side::LEFT, utils::translate(style::strings::common::options));

        activeApn        = std::make_shared<packet_data::APN::Config>();
        apnSettingsModel = std::make_shared<ApnSettingsModel>(application);
        apnSettingsModel->requestAPNList();
    }
    auto ApnSettingsWindow::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            LOG_ERROR("Received nullptr");
            return false;
        }

        const auto newData = dynamic_cast<ApnListData *>(data);
        if (newData == nullptr) {
            LOG_ERROR("Received nullptr");
            return false;
        }

        apns = newData->getAPNs();
        if (apns.empty()) {
            emptyListIcon->setVisible(true);
            return false;
        }

        return true;
    }

    void ApnSettingsWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        clearOptions();
        bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
        emptyListIcon->setVisible(false);

        if (apns.empty()) {
            return;
        }

        addOptions(optionsList(apns));

        if (mode == gui::ShowMode::GUI_SHOW_RETURN) {
            apnSettingsModel->requestAPNList();
        }

        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
    }

    auto ApnSettingsWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }
        if (!inputEvent.isShortRelease()) {
            return false;
        }
        if (inputEvent.is(gui::KeyCode::KEY_LEFT)) {
            auto apnRecord                        = std::make_shared<packet_data::APN::Config>();
            std::unique_ptr<gui::SwitchData> data = std::make_unique<ApnItemData>(apnRecord);
            application->switchWindow(gui::window::name::new_apn, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return true;
        }
        if (inputEvent.is(gui::KeyCode::KEY_LF)) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<ApnItemData>(activeApn);
            application->switchWindow(gui::window::name::apn_options, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return true;
        }

        return false;
    }

    auto ApnSettingsWindow::optionsList(std::vector<std::shared_ptr<packet_data::APN::Config>> apnsList)
        -> std::list<Option>
    {
        std::list<gui::Option> optionsList;

        for (const auto &apn : apnsList) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                (apn->apnType == packet_data::APN::APNType::Default) ? "<b>" + apn->apn + "</b>" : apn->apn,
                [=](gui::Item &item) {
                    LOG_DEBUG("APN: %s", apn->apn.c_str());
                    std::unique_ptr<gui::SwitchData> apnData = std::make_unique<ApnItemData>(apn);
                    application->switchWindow(
                        gui::window::name::new_apn, gui::ShowMode::GUI_SHOW_INIT, std::move(apnData));
                    return true;
                },
                [=](gui::Item &item) {
                    activeApn = apn;
                    return true;
                },
                nullptr));
        }

        return optionsList;
    }

} // namespace gui
