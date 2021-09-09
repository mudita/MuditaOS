// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>

#include <Translator.hpp>

namespace gui
{
    class InputLanguageWindow : public BaseSettingsWindow
    {
      public:
        explicit InputLanguageWindow(app::ApplicationCommon *app);

      protected:
        auto buildOptionsList() -> std::list<gui::Option> override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void setLanguageIndex();

      private:
        Profiles profiles;
        const std::vector<Language> langList;
        Language selectedLanguage;
        unsigned int selectedLanguageIndex{0};
    };
} // namespace gui
