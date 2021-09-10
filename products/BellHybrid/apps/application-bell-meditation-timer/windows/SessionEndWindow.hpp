// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "IconTextWindow.hpp"

namespace gui
{
    namespace name::window
    {
        inline constexpr auto session_end = "Session end";
    }

    class SessionEndWindow : public IconTextWindow
    {
      public:
        explicit SessionEndWindow(app::Application *app);

        // virtual methods
        void onTimeout() override;
        uint32_t getTimeout() override;
        std::vector<std::string> getText() override;
    };
} // namespace gui
