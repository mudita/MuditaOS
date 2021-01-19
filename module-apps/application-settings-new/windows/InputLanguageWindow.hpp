// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"
#include "Translator.hpp"

namespace gui
{

    class InputLanguageWindow : public BaseSettingsWindow
    {
      public:
        InputLanguageWindow(app::Application *app);

      protected:
        auto buildOptionsList() -> std::list<gui::Option> override;

      private:
        Language selectedLang;
        Profiles profiles;
    };
} // namespace gui
