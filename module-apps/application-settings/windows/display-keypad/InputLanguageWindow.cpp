// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InputLanguageWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{
    InputLanguageWindow::InputLanguageWindow(app::ApplicationCommon *app)
        : BaseSettingsWindow(app, window::name::input_language), langList(profiles.getAvailableInputLanguages())
    {
        setTitle(utils::translate("app_settings_display_input_language"));
    }

    void InputLanguageWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        selectedLanguage = utils::getInputLanguage();
        setLanguageIndex();

        refreshOptionsList(selectedLanguageIndex);
    }

    auto InputLanguageWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;
        for (const auto &lang : langList) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                lang,
                [=](gui::Item &item) {
                    selectedLanguage = lang;
                    app::manager::Controller::changeInputLanguage(application, lang);
                    refreshOptionsList();
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setNavBarText(utils::translate(style::strings::common::select), nav_bar::Side::Center);
                    }
                    return true;
                },
                this,
                selectedLanguage == lang ? gui::option::SettingRightItem::Checked
                                         : gui::option::SettingRightItem::Disabled));
        }

        return optionsList;
    }

    void InputLanguageWindow::setLanguageIndex()
    {
        for (unsigned int langIndex = 0; langIndex < langList.size(); ++langIndex) {
            if (selectedLanguage == langList[langIndex]) {
                selectedLanguageIndex = langIndex;
                break;
            }
        }
    }
} // namespace gui
