// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InputLanguageWindow.hpp"

#include "application-settings-new/ApplicationSettings.hpp"
#include "windows/OptionSetting.hpp"

#include <i18n/i18n.hpp>
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
        const auto &langList = loader.getAvailableDisplayLanguages();
        for (const auto &lang : langList) {
            optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
                lang,
                [=](gui::Item &item) {
                    selectedLang = lang;
                    app::manager::Controller::changeInputLanguage(application, lang);
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
                selectedLang == lang ? RightItem::Checked : RightItem::Disabled));
        }

        return optionsList;
    }
} // namespace gui
