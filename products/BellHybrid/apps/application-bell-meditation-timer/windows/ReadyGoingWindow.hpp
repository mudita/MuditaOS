// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "IconTextWindow.hpp"

namespace gui
{
    namespace name::window
    {
        inline constexpr auto ready_going = "Ready going";
    }

    class ReadyGoingWindow : public IconTextWindow
    {
      public:
        explicit ReadyGoingWindow(app::Application *app);

        // virtual methods
        void onTimeout() override;
        std::vector<std::string> getText() override;
        uint32_t getTimeout() override;
    };
} // namespace gui
