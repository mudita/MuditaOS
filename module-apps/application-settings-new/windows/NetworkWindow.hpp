// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionWindow.hpp"
#include "Application.hpp"

namespace gui
{

    namespace window
    {
        inline constexpr auto network_window = "Network";
    };

    class NetworkWindow : public OptionWindow
    {
      private:
        bool operatorsOn;
        auto netOptList() -> std::list<gui::Option>;
        void rebuildOptList();

      public:
        NetworkWindow(app::Application *app);
        void onBeforeShow(ShowMode m, SwitchData *d) override;
    };
} // namespace gui
