// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "presenter/RelaxationRunningLoopPresenter.hpp"
#include <data/RelaxationAudioData.hpp>

#include <AppWindow.hpp>
#include <common/widgets/BellStatusClock.hpp>
#include <common/widgets/BellBattery.hpp>

namespace gui
{
    class TextFixedSize;

    class RelaxationRunningLoopWindow : public AppWindow, public app::relaxation::RelaxationRunningLoopContract::View
    {
      public:
        RelaxationRunningLoopWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::relaxation::RelaxationRunningLoopContract::Presenter> &&presenter);

      private:
        std::unique_ptr<app::relaxation::RelaxationRunningLoopContract::Presenter> presenter;
        std::unique_ptr<RelaxationAudioContext> audioContext;

        BellStatusClock *clock{nullptr};
        TextFixedSize *titleText{nullptr};
        TextFixedSize *loopedText{nullptr};
        TextFixedSize *relaxationText{nullptr};
        BellBattery *battery{nullptr};

        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        RefreshModes updateTime() override;
        void onPaused() override;
        void resume() override;
        bool updateBatteryStatus() override;
        void handleError() override;
        void handleDeletedFile() override;

        void buildLayout();
    };
} // namespace gui
