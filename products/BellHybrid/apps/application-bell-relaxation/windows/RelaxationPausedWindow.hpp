// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/RelaxationPausedPresenter.hpp"

#include <AppWindow.hpp>

namespace gui
{
    class BellStatusClock;

    class RelaxationPausedWindow : public AppWindow, public app::relaxation::RelaxationPausedContract::View
    {
      public:
        RelaxationPausedWindow(app::ApplicationCommon *app,
                               std::unique_ptr<app::relaxation::RelaxationPausedContract::Presenter> &&presenter);

      private:
        std::unique_ptr<app::relaxation::RelaxationPausedContract::Presenter> presenter;

        gui::BellStatusClock *clock{nullptr};

        void buildInterface() override;
        void setTime(std::time_t newTime) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        bool onInput(const InputEvent &inputEvent) override;
        RefreshModes updateTime() override;
    };
} // namespace gui
