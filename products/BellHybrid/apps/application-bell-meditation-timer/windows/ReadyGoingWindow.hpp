// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ReadyGoingPresenter.hpp"

#include <apps-common/popups/WindowWithTimer.hpp>
#include <gui/widgets/Icon.hpp>

namespace gui
{
    class ReadyGoingWindow : public WindowWithTimer
    {
      private:
        std::shared_ptr<app::meditation::ReadyGoingPresenterContract::Presenter> presenter;

      public:
        ReadyGoingWindow(app::ApplicationCommon *app,
                         std::shared_ptr<app::meditation::ReadyGoingPresenterContract::Presenter> winPresenter);

        // virtual methods
        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;

      private:
        Icon *icon{};
    };
} // namespace gui
