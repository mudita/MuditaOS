// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"
#include <filesystem>

namespace gui
{
    class CheckBoxWithLabel;

    class QuotesMainWindow : public BaseSettingsWindow
    {
      public:
        QuotesMainWindow(app::Application *app);

        auto onInput(const InputEvent &inputEvent) -> bool override;

      protected:
        auto buildOptionsList() -> std::list<Option> override;

      private:
        void readQuotes(std::filesystem::path fn);
        void switchHandler(bool &optionSwitch);
        [[nodiscard]] static std::string readFileToString(const std::filesystem::path &fn);

        std::list<std::pair<std::string, bool>> quotes;
    };
} // namespace gui
