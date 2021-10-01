// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/options/BellShortOptionWindow.hpp>

namespace gui
{
    class BellTurnOffOptionWindow : public BellShortOptionWindow
    {
      public:
        static constexpr auto defaultName = "BellTurnOffOptionWindow";
        explicit BellTurnOffOptionWindow(app::ApplicationCommon *app, const char *name = defaultName);

      private:
        std::list<Option> settingsOptionsList();

        const UTF8 yesStr;
        const UTF8 noStr;
    };
} /* namespace gui */
