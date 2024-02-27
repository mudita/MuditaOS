// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationBellClock.hpp"
#include "presenter/BellClockWindowPresenter.hpp"

#include <apps-common/widgets/BarGraph.hpp>
#include <apps-common/widgets/TimeFixedWidget.hpp>
#include <module-gui/gui/widgets/Clock.hpp>

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class Text;
    class VBox;
    class BellBattery;
    class BellClockWindow : public AppWindow, public app::bell_clock::View
    {
      public:
        BellClockWindow(app::ApplicationCommon *app,
                        std::unique_ptr<app::bell_clock::BellClockWindowPresenter> &&windowPresenter,
                        std::string name = window::name::bellClock);

        void buildInterface() override;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        void rebuild() override;
        void exit() override;

      private:
        std::unique_ptr<app::bell_clock::BellClockWindowPresenter> presenter;
        gui::ArcProgressBar *progress{nullptr};
        gui::Clock *clock{nullptr};
        BellBattery *battery{nullptr};

        VBox *body{};
        Text *text{};
    };
} /* namespace gui */
