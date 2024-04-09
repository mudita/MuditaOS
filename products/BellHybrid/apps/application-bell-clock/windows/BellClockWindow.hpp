// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationBellClock.hpp"
#include "presenter/BellClockWindowPresenter.hpp"

#include <apps-common/widgets/TimeFixedWidget.hpp>
#include <module-gui/gui/widgets/Clock.hpp>

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class Text;
    class VBox;
    class BellBattery;
    class AlarmSetSpinner;

    class BellClockWindow : public AppWindow, public app::bell_clock::View
    {
      public:
        static constexpr auto name = "BellClockWindow";
        BellClockWindow(app::ApplicationCommon *app,
                        std::unique_ptr<app::bell_clock::BellClockWindowPresenter> &&windowPresenter,
                        std::string name = window::name::bellClock);

        void buildInterface() override;
        RefreshModes updateTime() override;
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void setTime(std::time_t time) override;

      private:
        std::unique_ptr<app::bell_clock::BellClockWindowPresenter> presenter;
        gui::Clock *clock{nullptr};
        BellBattery *battery{nullptr};
        AlarmSetSpinner *alarm{nullptr};

        VBox *bottom{};
        VBox *top{};
        VBox *dial[4];
        Text *text[4];
    };
} /* namespace gui */
