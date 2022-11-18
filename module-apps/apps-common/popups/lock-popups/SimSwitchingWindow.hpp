// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <popups/WindowWithTimer.hpp>
#include <Timers/TimerHandle.hpp>
#include <gui/widgets/Icon.hpp>

namespace gui
{
    class SimSwitchingWindow : public WindowWithTimer
    {
      private:
        Icon *infoIcon{nullptr};
        bool switchedSuccessful{false};

        void updateInterface();

      public:
        SimSwitchingWindow(app::ApplicationCommon *app, const std::string &name);

        void buildInterface() override;
    };
} // namespace gui
