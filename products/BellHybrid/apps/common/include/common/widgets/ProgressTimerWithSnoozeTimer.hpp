// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <apps-common/widgets/ProgressTimer.hpp>

namespace gui
{
    class SnoozeTimer;
} // namespace gui

namespace app
{
    class ProgressTimerWithSnoozeTimer : public ProgressTimer
    {
      public:
        using ProgressTimer::ProgressTimer;

        void attach(gui::SnoozeTimer *time);

      private:
        void update() override;
        void updateTime();

        gui::SnoozeTimer *timer{nullptr};
    };
} // namespace app
