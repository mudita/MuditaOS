// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/WindowNames.hpp>
#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace gui
{
    class LanguagesWindow : public BaseSettingsWindow
    {
      public:
        explicit LanguagesWindow(app::Application *app, std::string name = window::name::languages);

      protected:
        auto buildOptionsList() -> std::list<Option> override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void setLanguageIndex();

        utils::LangLoader loader;
        const std::vector<Language> langList;
        Language selectedLanguage;
        unsigned int selectedLanguageIndex{0};
    };
} /* namespace gui */
