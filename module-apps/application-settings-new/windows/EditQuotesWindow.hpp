// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"

namespace gui
{
    class CheckBoxWithLabel;

    class EditQuotesWindow : public BaseSettingsWindow
    {
      public:
        EditQuotesWindow(app::Application *app);

      private:
        void switchHandler(bool &optionSwitch);
        auto buildOptionsList() -> std::list<Option> override;

        bool isOurFavouritesSwitchOn = false;
        bool isCustomSwitchOn        = false;

        Item *quotes;
    };
} // namespace gui
