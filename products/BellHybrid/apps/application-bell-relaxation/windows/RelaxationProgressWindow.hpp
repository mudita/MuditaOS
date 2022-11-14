// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/RelaxationProgressPresenter.hpp"
#include "data/RelaxationAudioData.hpp"
#include <apps-common/widgets/TimeFixedWidget.hpp>
#include <AppWindow.hpp>
#include <common/widgets/BellStatusClock.hpp>

namespace gui
{
    class HBarGraph;
    class Text;
    class RelaxationProgressWindow : public AppWindow, public app::relaxation::RelaxationProgressContract::View
    {
        std::shared_ptr<app::relaxation::RelaxationProgressContract::Presenter> presenter;
        gui::Text *title                = nullptr;
        gui::HBarGraph *progressBar     = nullptr;
        gui::TimeFixedWidget *timerText = nullptr;
        gui::BellStatusClock *time      = nullptr;
        std::unique_ptr<RelaxationAudioContext> audioContext;

        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto onInput(const InputEvent &inputEvent) -> bool override;

        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        RefreshModes updateTime() override;

        void buildLayout();
        void configureTimer();

        void onFinished() override;
        void onPaused() override;

      public:
        RelaxationProgressWindow(app::ApplicationCommon *app,
                                 std::shared_ptr<app::relaxation::RelaxationProgressContract::Presenter> presenter);
    };
} // namespace gui
