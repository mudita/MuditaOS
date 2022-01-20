// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/WindowWithTimer.hpp>
#include <common/BellPowerOffPresenter.hpp>

namespace gui
{
    class Icon;

    class OnBoardingOnOffWindow : public WindowWithTimer
    {
      public:
        OnBoardingOnOffWindow(app::ApplicationCommon *app,
                              std::unique_ptr<AbstractPowerOffPresenter> presenter,
                              const std::string &name);

      private:
        constexpr static auto displayDuration = std::chrono::seconds{5};
        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;
        void buildLayout();
        void registerCallbacks();
        void powerOff();

        std::unique_ptr<AbstractPowerOffPresenter> presenter;
    };

} // namespace gui
