// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"

#include <i18n/i18n.hpp>

namespace gui
{
    namespace window::name
    {
        inline constexpr auto languages = "Languages";
    } /* namespace window::name */

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
