// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "data/FocusCommon.hpp"
#include "presenter/FocusTimerPresenter.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace app::focus
{
    class FocusTimerWindow : public gui::AppWindow, public FocusTimerContract::View
    {
      public:
        FocusTimerWindow(app::ApplicationCommon *app,
                         std::unique_ptr<FocusTimerContract::Presenter> &&windowPresenter,
                         const std::string &name = window::name::timer);

        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;

      private:
        std::unique_ptr<FocusTimerContract::Presenter> presenter;
    };
} // namespace app::focus
