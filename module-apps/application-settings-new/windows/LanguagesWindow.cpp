// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/data/LanguagesData.hpp"
#include "LanguagesWindow.hpp"
#include "OptionSetting.hpp"
#include "service-appmgr/Controller.hpp"

namespace gui
{
    LanguagesWindow::LanguagesWindow(app::Application *app)
        : BaseSettingsWindow(app, window::name::languages), langList(loader.getAvailableDisplayLanguages()),
          languagesModel(app)
    {
        setTitle(utils::localize.get("app_settings_title_languages"));
        languagesModel.requestCurrentDisplayLanguage();
    }

    void LanguagesWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        refreshOptionsList(selectedLanguageIndex);
    }

    auto LanguagesWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> options;

        for (const auto &lang : langList) {
            options.emplace_back(std::make_unique<gui::option::OptionSettings>(
                lang,
                [=](const gui::Item &item) {
                    app::manager::Controller::changeDisplayLanguage(application, lang);
                    return true;
                },
                nullptr,
                this,
                selectedLanguage == lang ? gui::option::SettingRightItem::Checked
                                         : gui::option::SettingRightItem::Disabled));
        }

        return options;
    }

    auto LanguagesWindow::handleSwitchData(SwitchData *data) -> bool
    {
        auto *languagesData = dynamic_cast<LanguagesData *>(data);
        if (languagesData == nullptr) {
            return false;
        }

        selectedLanguage = languagesData->getCurrentDisplayLanguage();
        setLanguageIndex();

        return true;
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
