// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "MeditationCountdownPresenter.hpp"

#include <Application.hpp>
#include <AppWindow.hpp>
#include <InputEvent.hpp>

namespace gui
{
    class Arc;
    class TimeMinuteSecondWidget;

    class MeditationCountdownWindow : public AppWindow, public app::meditation::MeditationCountdownContract::View
    {
      public:
        MeditationCountdownWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::meditation::MeditationCountdownContract::Presenter> &&windowPresenter);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;

      private:
        std::unique_ptr<app::meditation::MeditationCountdownContract::Presenter> presenter;
        VBox *mainVBox{nullptr};
        Arc *progress{nullptr};
        TimeMinuteSecondWidget *timer{nullptr};
        Text *description{nullptr};
        TextFixedSize *bottomDescription{nullptr};

        void buildLayout();
        void configureTimer();
    };
} // namespace gui
