// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace gui
{
    class CheckBoxWithLabel;

    class EditQuotesWindow : public BaseSettingsWindow
    {
      public:
        explicit EditQuotesWindow(app::ApplicationCommon *app);

      private:
        void switchHandler(bool &optionSwitch);
        auto buildOptionsList() -> std::list<Option> override;

        Item *quotes;
        OptionWindowDestroyer rai_destroyer = OptionWindowDestroyer(*this);
    };
} // namespace gui
