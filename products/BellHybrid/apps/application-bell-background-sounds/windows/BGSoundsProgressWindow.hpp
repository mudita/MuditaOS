// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/BGSoundsProgressPresenter.hpp"
#include "data/BGSoundsAudioData.hpp"
#include <AppWindow.hpp>

namespace gui
{
    class HBarGraph;
    class Text;
    class BGSoundsProgressWindow : public AppWindow, public app::bgSounds::BGSoundsProgressContract::View
    {
        std::shared_ptr<app::bgSounds::BGSoundsProgressContract::Presenter> presenter;
        gui::Text *title            = nullptr;
        gui::HBarGraph *progressBar = nullptr;
        gui::Text *timerText        = nullptr;
        std::unique_ptr<BGSoundsAudioContext> audioContext;

        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto onInput(const InputEvent &inputEvent) -> bool override;

        void buildLayout();
        void configureTimer();

        void onFinished() override;
        void onPaused() override;

      public:
        BGSoundsProgressWindow(app::ApplicationCommon *app,
                               std::shared_ptr<app::bgSounds::BGSoundsProgressContract::Presenter> presenter);
    };
} // namespace gui
