// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <InputEvent.hpp>
#include <Label.hpp>
#include <Image.hpp>

#include "MeditationWindow.hpp"
#include "IntervalChimePresenter.hpp"

namespace gui
{
    namespace name::window
    {
        inline constexpr auto interval_chime = "Interval chime";
    }

    class IntervalChimeWindow : public MeditationWindow, public app::meditation::IntervalChimeContract::View
    {
      public:
        explicit IntervalChimeWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::meditation::IntervalChimeContract::Presenter> &&windowPresenter);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void updateDisplay() override;
        void buildMeditationItem(MeditationItem &item) override;
        void onMeditationItemAvailable(MeditationItem *item) override;

      private:
        std::unique_ptr<app::meditation::IntervalChimeContract::Presenter> presenter;
        gui::Image *previousImage = nullptr;
        gui::Image *nextImage     = nullptr;
        gui::Label *title         = nullptr;
        gui::Label *text          = nullptr;
        gui::Label *minute        = nullptr;
    };
} // namespace gui
