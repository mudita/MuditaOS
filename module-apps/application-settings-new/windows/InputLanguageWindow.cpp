// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InputLanguageWindow.hpp"

#include "application-settings-new/ApplicationSettings.hpp"
#include "windows/OptionSetting.hpp"

#include <i18/i18.hpp>
#include <module-services/service-appmgr/service-appmgr/Controller.hpp>

namespace gui
{
    InputLanguageWindow::InputLanguageWindow(app::Application *app)
        : BaseSettingsWindow(app, window::name::input_language)
    {
        setTitle(utils::localize.get("app_settings_display_input_language"));
    }

    auto InputLanguageWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;
        std::vector<std::string> languageListJson = {"app_settings_language_english",
                                                     "app_settings_language_polish",
                                                     "app_settings_language_german",
                                                     "app_settings_language_spanish"};
        std::vector<utils::Lang> languageListEnum = {
            utils::Lang::En, utils::Lang::Pl, utils::Lang::De, utils::Lang::Sp};

        for (size_t i = 0; i < languageListJson.size(); i++) {
            optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
                utils::localize.get(languageListJson[i]),
                [=](gui::Item &item) {
                    selectedLang = utils::localize.get(languageListJson[i]);
                    app::manager::Controller::changeInputLanguage(application, languageListEnum[i]);
                    rebuildOptionList();
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::translateI18(style::strings::common::select),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                this,
                selectedLang == utils::localize.get(languageListJson[i]) ? RightItem::Checked : RightItem::Disabled));
        }

        return optionsList;
    }
} // namespace gui
