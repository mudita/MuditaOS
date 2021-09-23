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
        explicit SessionEndWindow(app::ApplicationCommon *app);

        // virtual methods
        void onTimeout() override;
        std::string getText() override;
        std::string getImageName() override;
        std::chrono::seconds getTimeout() const override;

      private:
        static constexpr std::chrono::seconds timeout = std::chrono::seconds{3};
    };
} // namespace gui
