// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApnOptionsWindow.hpp"

#include <application-settings/data/SettingsItemData.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>

namespace gui
{
    ApnOptionsWindow::ApnOptionsWindow(app::ApplicationCommon *app)
        : BaseSettingsWindow(app, window::name::apn_options), apnSettingsModel(app)
    {
        setTitle(utils::translate("common_options_title"));
    }

    auto ApnOptionsWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_apn_options_edit"),
            [=](gui::Item &item) {
                std::unique_ptr<gui::SwitchData> data = std::make_unique<ApnItemData>(apn);
                data->ignoreCurrentWindowOnStack      = true;
                application->switchWindow(gui::window::name::new_apn, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                return true;
            },
            nullptr,
            this));

        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_apn_options_delete"),
            [=](gui::Item &item) {
                apnSettingsModel.removeAPN(apn);
                application->returnToPreviousWindow();
                return true;
            },
            nullptr,
            this));

        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_apn_options_set_as_default"),
            [=](gui::Item &item) {
                apnSettingsModel.setAsDefaultAPN(apn);
                application->returnToPreviousWindow();
                return true;
            },
            nullptr,
            this));

        return optionsList;
    }

    auto ApnOptionsWindow::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            return false;
        }

        auto item = dynamic_cast<ApnItemData *>(data);
        if (item == nullptr) {
            return false;
        }

        apn = item->getApn();
        if (apn == nullptr) {
            apn = std::make_shared<packet_data::APN::Config>();
            return true;
        }

        return true;
    }
} // namespace gui
