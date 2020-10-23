// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"

namespace gui
{
    class CheckBoxWithLabel;

    class QuotesMainWindow : public BaseSettingsWindow
    {
      public:
        QuotesMainWindow(app::Application *app);
        void buildInterface() override;

        bool onInput(const InputEvent &inputEvent) override;

      private:
        void buildQuotesList(std::list<std::string> quotes);

        std::vector<CheckBoxWithLabel *> boxes;
        std::list<std::string> readQuotes(fs::path fn);
    };
} // namespace gui
