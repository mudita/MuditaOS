// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <common/options/BellShortOptionWindow.hpp>
#include <apps-common/ApplicationCommon.hpp>

namespace style::bell_shortcuts_layout
{
    inline constexpr auto top_layout_h    = 153U;
    inline constexpr auto center_layout_h = 180U;
    inline constexpr auto bottom_layout_h = 51U;
} // namespace style::bell_shortcuts_layout

namespace gui
{
    class OnBoardingShortcutsOptionWindow : public BellShortOptionWindow
    {
      public:
        explicit OnBoardingShortcutsOptionWindow(app::ApplicationCommon *app, const std::string &name);

      private:
        std::list<Option> settingsOptionsList();

        const UTF8 startString;
        const UTF8 skipString;
    };
} /* namespace gui */
