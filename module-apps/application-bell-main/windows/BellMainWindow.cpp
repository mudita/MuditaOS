// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellMainWindow.hpp"

#include <application-bell-main/ApplicationBellMain.hpp>
#include <data/BellMainStyle.hpp>

#include <gui/input/InputEvent.hpp>
#include <i18n/i18n.hpp>
#include <log/log.hpp>
#include <service-appmgr/Controller.hpp>
#include <time/time_conversion.hpp>

namespace gui
{
    BellMainWindow::BellMainWindow(app::Application *app,
                                   std::unique_ptr<BellAlarmWindowContract::Presenter> &&windowPresenter)
        : AppWindow(app, gui::name::window::main_window), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void BellMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        // for test purposes only - to be replaced by AlarmSetSpinner widget
        // do not pay attention to magic numbers in gui primitives init

        auto vBox = new gui::VBox(this,
                                  style::window::default_left_margin,
                                  45,
                                  style::window::default_body_width,
                                  style::window::default_body_height);
        vBox->setEdges(gui::RectangleEdge::None);

        auto hBox = new gui::HBox();
        hBox->setEdges(gui::RectangleEdge::None);
        hBox->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        hBox->setMinimumSize(style::window::default_body_width, 64);

        auto leftArrow = new Image(hBox, 0, 0, "bell_arrow_left");
        leftArrow->setAlignment(Alignment(gui::Alignment::Vertical::Center));
        leftArrow->setMargins(Margins(0, 0, 5, 0));

        Image *alarmImg;
        auto alarmStatus = presenter->getAlarmStatus();
        switch (alarmStatus) {
        case BellAlarm::Status::ACTIVATED:
            alarmImg = new Image(hBox, 0, 0, "bell_alarm_activated");
            break;
        case BellAlarm::Status::DEACTIVATED:
            alarmImg = new Image(hBox, 0, 0, "bell_alarm_deactivated");
            break;
        case BellAlarm::Status::RINGING:
            alarmImg = new Image(hBox, 0, 0, "bell_alarm_ringing");
            break;
        case BellAlarm::Status::SNOOZE:
            alarmImg = new Image(hBox, 0, 0, "bell_alarm_snooze");
            break;
        case BellAlarm::Status::UNKNOWN:
            alarmImg = new Image(hBox, 0, 0, "bell_alarm_deactivated");
            break;
        default:
            alarmImg = new Image(hBox, 0, 0, "bell_alarm_deactivated");
            break;
        }

        alarmImg->setAlignment(Alignment(gui::Alignment::Vertical::Center));
        alarmImg->setMargins(Margins(0, 0, 5, 0));

        auto TimeLabel = new gui::Label();
        TimeLabel->setFont(style::window::font::largelight);
        TimeLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        TimeLabel->setMinimumSize(200, 64);
        TimeLabel->setEdges(gui::RectangleEdge::None);
        hBox->addWidget(TimeLabel);

        auto rightArrow = new Image(hBox, 0, 0, "bell_arrow_right");
        rightArrow->setAlignment(Alignment(gui::Alignment::Vertical::Center));
        rightArrow->setMargins(Margins(0, 0, 5, 0));

        vBox->addWidget(hBox);
        std::string timeString = std::to_string(presenter->getHour()) + ":" + std::to_string(presenter->getMinute());
        TimeLabel->setText(timeString);

        namespace timeLabel = bellMainStyle::mainWindow::timeLabel;
        time = new gui::Label(this, timeLabel::posX, timeLabel::posY, timeLabel::width, timeLabel::height);
        time->setFilled(false);
        time->setBorderColor(gui::ColorNoColor);
        time->setFont(timeLabel::font);
        time->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        namespace temperatureLabel = bellMainStyle::mainWindow::temperatureLabel;
        temperature                = new gui::Label(
            this, temperatureLabel::posX, temperatureLabel::posY, temperatureLabel::width, temperatureLabel::height);
        temperature->setFilled(false);
        temperature->setBorderColor(gui::ColorNoColor);
        temperature->setFont(temperatureLabel::font);
        temperature->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        std::string degree_sign(temperatureLabel::degree);
        temperature->setText("12 " + degree_sign + "C");
    }

    bool BellMainWindow::onInput(const InputEvent &inputEvent)
    {
        LOG_INFO("Bell Main Window Input");
        if (inputEvent.isShortRelease()) {
            switch (inputEvent.getKeyCode()) {
            case KeyCode::KEY_ENTER:
                LOG_INFO("Open MainMenu");
                application->switchWindow(gui::window::name::bell_main_menu, nullptr);
                return true;
            case KeyCode::KEY_RF:
                return true;
            default:
                break;
            }
        }

        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    bool BellMainWindow::updateTime()
    {
        if (time != nullptr) {
            utils::time::Timestamp timestamp{std::time(nullptr)};
            time->setText(timestamp.str("%H:%M"));
            return true;
        }

        return false;
    }

} // namespace gui
