// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <InputEvent.hpp>
#include <SideListView.hpp>

#include "MeditationWindow.hpp"
#include "MeditationTimerPresenter.hpp"

namespace gui
{
    namespace name::window
    {
        inline constexpr auto meditation_timer = "Meditation timer";
    }

    class MeditationTimerWindow : public MeditationWindow, public app::meditation::MeditationTimerContract::View
    {
      public:
        explicit MeditationTimerWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::meditation::MeditationTimerContract::Presenter> &&windowPresenter);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;
        void buildMeditationItem(MeditationItem &item) override;
        void onMeditationItemAvailable(MeditationItem *item) override;

      private:
        std::unique_ptr<app::meditation::MeditationTimerContract::Presenter> presenter;
        SideListView *sideListView = nullptr;
    };
} // namespace gui
