// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <common/BellPowerOffPresenter.hpp>
#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    class Icon;

    class BellTurnOffWindow : public WindowWithTimer
    {
      public:
        static constexpr auto name = "BellTurnOffWindow";

        BellTurnOffWindow(app::ApplicationCommon *app, std::unique_ptr<AbstractPowerOffPresenter> presenter);

      private:
        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;

        Icon *icon{};
        std::unique_ptr<AbstractPowerOffPresenter> presenter;
    };

} // namespace gui
