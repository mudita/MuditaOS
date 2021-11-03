// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    class Icon;

    class OnBoardingWelcomeWindow : public WindowWithTimer
    {
      public:
        OnBoardingWelcomeWindow(app::ApplicationCommon *app, const std::string &name);

      private:
        constexpr static auto displayDuration = std::chrono::seconds{5};
        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void buildLayout();
        void registerCallbacks();
        Icon *icon{};
    };

} // namespace gui
