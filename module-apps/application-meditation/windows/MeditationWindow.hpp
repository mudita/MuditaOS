// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include "MeditationParams.hpp"

namespace gui
{
    class IntervalBox;
    class TimerProperty;

    class MeditationWindow : public AppWindow
    {
      public:
        MeditationWindow(app::ApplicationCommon *app, const MeditationParams &params);

        auto onInput(const InputEvent &inputEvent) -> bool override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        void invalidate() noexcept;
        TimerProperty *timeSetter = nullptr;
        IntervalBox *intervalBox  = nullptr;
        MeditationParams meditationParams;
    };
} // namespace gui
