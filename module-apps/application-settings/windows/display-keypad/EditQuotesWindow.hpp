// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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

        bool isOurFavouritesSwitchOn = false;
        bool isCustomSwitchOn        = false;

        [[maybe_unused]] Item *quotes;
    };
} // namespace gui
