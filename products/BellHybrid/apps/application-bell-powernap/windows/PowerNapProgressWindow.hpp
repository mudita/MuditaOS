// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/PowerNapProgressPresenter.hpp"

#include <AppWindow.hpp>

namespace gui
{
    class Text;
    class ArcProgressBar;
    class TimeMinuteSecondWidget;
    class BellStatusClock;

    class PowerNapProgressWindow : public AppWindow, public app::powernap::PowerNapProgressContract::View
    {
      public:
        PowerNapProgressWindow(app::ApplicationCommon *app,
                               std::shared_ptr<app::powernap::PowerNapProgressContract::Presenter> &&windowPresenter);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;
        void progressFinished() override;
        void pause() override;
        void resume() override;

      private:
        std::shared_ptr<app::powernap::PowerNapProgressContract::Presenter> presenter;

        VBox *mainVBox{nullptr};
        BellStatusClock *clock{nullptr};
        ArcProgressBar *progress{nullptr};
        VBox *pauseBox{nullptr};
        VBox *ringBox{nullptr};
        TimeMinuteSecondWidget *timer{nullptr};
        TextFixedSize *bottomDescription{nullptr};

        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        RefreshModes updateTime() override;
        void buildLayout();
        void configureTimer();

        void napEnded() override;
    };
} // namespace gui
