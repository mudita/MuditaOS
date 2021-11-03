// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/widgets/spinners/Spinners.hpp>
#include <widgets/BellBaseLayout.hpp>

#include <Application.hpp>
#include <AppWindow.hpp>
#include <InputEvent.hpp>

#include "MeditationTimerPresenter.hpp"

namespace gui
{
    class MeditationTimerWindow : public AppWindow, public app::meditation::MeditationTimerContract::View
    {
      public:
        explicit MeditationTimerWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::meditation::MeditationTimerContract::Presenter> &&windowPresenter);

        // virtual methods
        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;

        void onValueChanged(const std::uint32_t currentValue);

      private:
        std::unique_ptr<app::meditation::MeditationTimerContract::Presenter> presenter;
        BellBaseLayout *body{};
        UIntegerSpinner *spinner{};
        Label *bottomDescription{};
    };
} // namespace gui
