// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <AppWindow.hpp>
#include <InputEvent.hpp>
#include <Text.hpp>
#include <apps-common/widgets/TimeFixedWidget.hpp>

#include "MeditationCountdownPresenter.hpp"

namespace gui
{
    class Arc;

    class MeditationCountdownWindow : public AppWindow, public app::meditation::MeditationCountdownContract::View
    {
      public:
        MeditationCountdownWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::meditation::MeditationCountdownContract::Presenter> &&windowPresenter);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;

      private:
        std::unique_ptr<app::meditation::MeditationCountdownContract::Presenter> presenter;
        VBox *mainVBox    = nullptr;
        Arc *progress     = nullptr;
        TimeFixedWidget *timer = nullptr;
        Text *description = nullptr;

        void buildLayout();
        void configureTimer();

        void endSession();
        void intervalTimeout();
        void playGong();
    };
} // namespace gui
