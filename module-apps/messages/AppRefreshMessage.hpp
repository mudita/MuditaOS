// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppMessage.hpp"

namespace app
{

    class AppRefreshMessage : public AppMessage
    {
      protected:
        gui::RefreshModes mode;
        std::string window_name;

      public:
        AppRefreshMessage(gui::RefreshModes mode, std::string window_name)
            : mode{mode}, window_name(std::move(window_name)){};

        [[nodiscard]] const gui::RefreshModes &getMode() const
        {
            return mode;
        }

        [[nodiscard]] const std::string &getWindowName() const
        {
            return window_name;
        }

        operator std::string() const override;
    };
} // namespace app
