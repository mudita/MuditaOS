// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-settings-new/models/LanguagesModel.hpp"
#include "BaseSettingsWindow.hpp"
#include <i18n/i18n.hpp>

namespace gui
{
    class LanguagesWindow : public BaseSettingsWindow
    {
      public:
        explicit LanguagesWindow(app::Application *app);

      private:
        auto buildOptionsList() -> std::list<Option> override;
        auto handleSwitchData(SwitchData *data) -> bool override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void setLanguageIndex();

        utils::LangLoader loader;
        const std::vector<Language> langList;
        LanguagesModel languagesModel;
        Language selectedLanguage;
        unsigned int selectedLanguageIndex{0};
    };
} /* namespace gui */
