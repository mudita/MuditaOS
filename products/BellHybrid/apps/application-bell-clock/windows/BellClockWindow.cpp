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

#include <Style.hpp>

namespace
{
    auto batteryValue = 50;

    constexpr auto font   = style::window::font::largelight;

    namespace topBox
    {
        constexpr auto width  = 460U;
        constexpr auto height = 460U;
        constexpr auto xPos   = 70U;
        constexpr auto yPos   = 10U;
    } // namespace topBox

    namespace bottomBox
    {
        constexpr auto width  = 240U;
        constexpr auto height = 100U;
    } // namespace bottomBox

    namespace dial
    {
        constexpr auto font = style::window::font::veryverybiglight;

        constexpr auto textWidth   = 58U;
        constexpr auto textHeight  = 46U;
        constexpr auto topPosY     = 16U;
        constexpr auto centerTextX = (600U / 2U) - (textWidth / 2);
        constexpr auto centerTextY = (480U / 2U) - (textHeight / 2);
        constexpr auto rightTextX  = centerTextX + (topBox::width / 2) - 36U;
        constexpr auto bottomPosY  = centerTextY + (topBox::height / 2) - 30U;
        constexpr auto leftTextX   = topBox::xPos + 8U;
    } // namespace dial
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

        top = new VBox(this, topBox::xPos, topBox::yPos, topBox::width, topBox::height);
        top->setEdges(RectangleEdge::None);

        alarm = new AlarmSetSpinner(top);
        alarm->setMinimumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        alarm->setFont(font);
        alarm->setEditMode(EditMode::Browse);
        alarm->setAlarmStatus(AlarmSetSpinner::Status::ACTIVATED);
        alarm->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        alarm->setMargins(gui::Margins(0, 100U, 0, 0));
        alarm->setTime(now);
        alarm->setVisible(true);
        alarm->setAlarmTimeVisible(true);

        dial[0] = new VBox(this, dial::centerTextX, dial::topPosY, dial::textWidth, dial::textHeight);
        dial[0]->setEdges(RectangleEdge::None);
        dial[1] = new VBox(this, dial::rightTextX, dial::centerTextY, dial::textWidth, dial::textHeight);
        dial[1]->setEdges(RectangleEdge::None);
        dial[2] = new VBox(this, dial::centerTextX, dial::bottomPosY, dial::textWidth, dial::textHeight);
        dial[2]->setEdges(RectangleEdge::None);
        dial[3] = new VBox(this, dial::leftTextX, dial::centerTextY, dial::textWidth, dial::textHeight);
        dial[3]->setEdges(RectangleEdge::None);

        text[0] = new Text(dial[0], dial::centerTextX, dial::topPosY, dial::textWidth, dial::textHeight);
        text[0]->setText("12");
        text[0]->setFont(dial::font);
        text[0]->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        text[1] = new Text(dial[1], dial::centerTextX, dial::topPosY, dial::textWidth, dial::textHeight);
        text[1]->setText("3");
        text[1]->setFont(dial::font);
        text[1]->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        text[2] = new Text(dial[2], dial::centerTextX, dial::topPosY, dial::textWidth, dial::textHeight);
        text[2]->setText("6");
        text[2]->setFont(dial::font);
        text[2]->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        text[3] = new Text(dial[3], dial::centerTextX, dial::topPosY, dial::textWidth, dial::textHeight);
        text[3]->setText("9");
        text[3]->setFont(dial::font);
        text[3]->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        bottom = new VBox(this, 180U, 280U, bottomBox::width, bottomBox::height);
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
        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            application->returnToPreviousWindow();
        }
        return AppWindow::onInput(inputEvent);
    }

    void BellClockWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellClockWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (presenter != nullptr) {
            presenter->updateTime();
        }
    }

    RefreshModes BellClockWindow::updateTime()
    {
        if (presenter != nullptr) {
            return presenter->handleUpdateTimeEvent();
        }
        return RefreshModes::GUI_REFRESH_NONE;
    }

    void BellClockWindow::setTime(std::time_t time)
    {
        struct tm *t = gmtime(&time);
        clock->setTime(t->tm_hour % 12, t->tm_min);
    }
} /* namespace gui */
