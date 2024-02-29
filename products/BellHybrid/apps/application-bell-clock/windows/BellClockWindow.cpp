// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellClockWindow.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/TimeSetFmtSpinner.hpp>

#include <module-gui/gui/input/InputEvent.hpp>

#include <module-gui/gui/widgets/text/TextFixedSize.hpp>
#include <module-gui/gui/widgets/ThreeBox.hpp>
#include <module-gui/gui/widgets/Image.hpp>

#include <widgets/SideListView.hpp>
#include <widgets/AlarmSetSpinner.hpp>
#include <common/widgets/BellBattery.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>

namespace
{
    auto batteryValue = 50;
    inline constexpr auto font = style::window::font::largelight;
}

namespace gui
{
    BellClockWindow::BellClockWindow(app::ApplicationCommon *app,
                                     std::unique_ptr<app::bell_clock::BellClockWindowPresenter> &&windowPresenter,
                                     std::string name)
        : AppWindow(app, std::move(name)), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void BellClockWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(true);
        navBar->setVisible(false);

        Arc::ShapeParams arcParams;
        arcParams.setCenterPoint(Point(getWidth() / 2, getHeight() / 2))
            .setRadius(192U)
            .setStartAngle(0U)
            .setSweepAngle(360U)
            .setPenWidth(1U)
            .setBorderColor(ColorFullBlack);

        progress = new ArcProgressBar(this,
                                      arcParams,
                                      ArcProgressBar::ProgressDirection::CounterClockwise,
                                      ArcProgressBar::ProgressChange::DecrementFromFull);
        progress->setMaximum(3);

        top = new VBox(this, 100, 120, 400, 120);
        top->setEdges(RectangleEdge::None);

        alarm = new AlarmSetSpinner(top);
        alarm->setMinimumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        alarm->setFont(font);
        alarm->setEditMode(EditMode::Browse);
        alarm->setAlarmStatus(AlarmSetSpinner::Status::ACTIVATED);
        alarm->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        alarm->setTime(now);
        alarm->setVisible(true);
        alarm->setAlarmTimeVisible(true);

        bottom = new VBox(this, 180, 300, 240, 100);
        bottom->setEdges(RectangleEdge::None);

        battery = new BellBattery(bottom, gui::BatteryWidthMode::FitToContent);
        battery->setMargins(Margins(0U, style::bell_base_layout::info_box_top_margin, 0U, 0U));
        battery->setMaximumSize(battery::battery_widget_w, battery::battery_widget_h);
        battery->setEdges(RectangleEdge::None);
        battery->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        battery->update(batteryValue, false);
        battery->setVisible(true);
        battery->setBatteryPercentMode(BatteryPercentMode::Show);

        clock = new Clock(this); // This must be at the end of the command list to draw

        bottom->resizeItems();
        top->resizeItems();
    }

    bool BellClockWindow::onInput(const InputEvent &inputEvent)
    {
        if (clock->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            battery->setVisible(true);
            battery->update(batteryValue++, false);
            battery->informContentChanged();
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    void BellClockWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellClockWindow::exit()
    {
        application->switchWindow(
            gui::window::bell_finished::defaultName,
            BellFinishedWindowData::Factory::create("circle_success_big",
                                                    "",
                                                    "",
                                                    BellFinishedWindowData::ExitBehaviour::CloseApplication,
                                                    std::chrono::seconds::zero()));
    }
} /* namespace gui */
