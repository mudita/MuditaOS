// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "AppWindow.hpp"

namespace gui
{
    class StatusBarImageTypeWindow : public AppWindow
    {
      public:
        explicit StatusBarImageTypeWindow(app::ApplicationCommon *app);

        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;
    };
} /* namespace gui */
