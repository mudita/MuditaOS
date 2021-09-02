// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationBellAlarm.hpp"
#include "presenter/BellAlarmWindowPresenter.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class BellBaseLayout;
    class TimeSetFmtSpinner;

    class BellAlarmWindow : public AppWindow, public app::bell_alarm::BellAlarmWindowContract::View
    {
      public:
        explicit BellAlarmWindow(app::Application *app,
                                 std::unique_ptr<app::bell_alarm::BellAlarmWindowContract::Presenter> &&windowPresenter,
                                 std::string name = window::name::bellAlarm);

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;

      private:
        BellBaseLayout *body{nullptr};
        Label *topText{nullptr};
        TimeSetFmtSpinner *timeSetFmtSpinner{nullptr};
        std::unique_ptr<app::bell_alarm::BellAlarmWindowContract::Presenter> presenter;
    };
} /* namespace gui */
