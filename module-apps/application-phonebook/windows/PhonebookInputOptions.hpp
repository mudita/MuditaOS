// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionWindow.hpp"

namespace gui
{
    class PhonebookInputOptions : public OptionWindow
    {
      public:
        PhonebookInputOptions(app::ApplicationCommon *app, std::string windowName);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        auto inputOptionsList(gui::Text *text) -> std::list<gui::Option>;
    };
} // namespace gui
