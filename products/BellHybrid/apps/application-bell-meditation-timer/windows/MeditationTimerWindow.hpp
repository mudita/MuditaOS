// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "MeditationTimerPresenter.hpp"

#include <Application.hpp>
#include <AppWindow.hpp>
#include <InputEvent.hpp>

#include <apps-common/widgets/spinners/Spinners.hpp>

namespace gui
{
    class BellBaseLayout;
}

namespace app::meditation
{
    using namespace gui;

    class MeditationTimerWindow : public AppWindow, public app::meditation::MeditationTimerContract::View
    {
      public:
        static constexpr auto name = "MeditationTimerWindow";

        explicit MeditationTimerWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::meditation::MeditationTimerContract::Presenter> &&windowPresenter);

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;

        void onValueChanged(std::uint32_t currentValue);

      private:
        std::unique_ptr<app::meditation::MeditationTimerContract::Presenter> presenter;
        BellBaseLayout *body{nullptr};
        U8IntegerSpinner *spinner{nullptr};
        Label *bottomDescription{nullptr};
    };
} // namespace app::meditation
