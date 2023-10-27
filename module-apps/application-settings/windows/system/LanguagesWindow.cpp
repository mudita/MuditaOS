// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LanguagesWindow.hpp"

#include <OptionSetting.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{
    LanguagesWindow::LanguagesWindow(app::ApplicationCommon *app, std::string name)
        : BaseSettingsWindow(app, std::move(name)), langList(utils::getAvailableDisplayLanguages())
    {
        setTitle(utils::translate("app_settings_title_languages"));
    }

    void LanguagesWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        selectedLanguage = utils::getDisplayLanguage();
        setLanguageIndex();

        refreshOptionsList(selectedLanguageIndex);
    }

    auto LanguagesWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> options;

        for (const auto &lang : langList) {
            options.emplace_back(std::make_unique<gui::option::OptionSettings>(
                lang,
                [=](const gui::Item &item) {
                    // Changing the system language will change the input language
                    app::manager::Controller::changeDisplayLanguage(application, lang);
                    app::manager::Controller::changeInputLanguage(application, lang);
                    return true;
                },
                nullptr,
                this,
                selectedLanguage == lang ? gui::option::SettingRightItem::Checked
                                         : gui::option::SettingRightItem::Disabled));
        }

        return options;
    }

    void LanguagesWindow::setLanguageIndex()
    {
        for (unsigned int langIndex = 0; langIndex < langList.size(); ++langIndex) {
            if (selectedLanguage == langList[langIndex]) {
                selectedLanguageIndex = langIndex;
                break;
            }
        }
    }
} /* namespace gui */
