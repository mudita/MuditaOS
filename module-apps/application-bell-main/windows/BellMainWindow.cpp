// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellMainWindow.hpp"
#include <application-bell-main/ApplicationBellMain.hpp>
#include <data/BellMainStyle.hpp>
#include <gui/input/InputEvent.hpp>
#include <i18n/i18n.hpp>
#include <log/log.hpp>
#include <time/time_conversion.hpp>

static constexpr uint32_t mockHour   = 10;
static constexpr uint32_t mockMinute = 15;

namespace gui
{
    BellMainWindow::BellMainWindow(app::Application *app) : AppWindow(app, gui::name::window::main_window)
    {
        buildInterface();
    }

    void BellMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        auto vBox = new gui::VBox(this,
                                  bellMainStyle::mainWindow::body::posX,
                                  bellMainStyle::mainWindow::body::posY,
                                  style::window::default_body_width,
                                  style::window::default_body_height);
        vBox->setEdges(gui::RectangleEdge::None);

        alarmSetSpinner = new AlarmSetSpinner(this, 0, 0, style::alarm_set_spinner::w, style::alarm_set_spinner::h);
        alarmSetSpinner->setMinimumSize(style::window::default_body_width, style::alarm_set_spinner::h);
        // for test purposes only
        alarmSetSpinner->setHour(mockHour);
        alarmSetSpinner->setMinute(mockMinute);

        alarmSetSpinner->setFont(style::window::font::largelight);
        alarmSetSpinner->setEditMode(EditMode::Browse);
        alarmStatus = BellAlarm::Status::DEACTIVATED;
        alarmSetSpinner->setAlarmStatus(alarmStatus);
        vBox->addWidget(alarmSetSpinner);

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

        setFocusItem(time);
    }

    bool BellMainWindow::onInput(const InputEvent &inputEvent)
    {
        LOG_INFO("Bell Main Window Input");
        if (inputEvent.isShortRelease()) {
            switch (inputEvent.getKeyCode()) {
            case KeyCode::KEY_ENTER:
                return handleEnterKey(inputEvent);
            case KeyCode::KEY_UP:
            case KeyCode::KEY_DOWN:
                handleEditModeEntry();
                break;

            default:
                break;
            }
        }
        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    bool BellMainWindow::handleEnterKey(const InputEvent &inputEvent)
    {
        if (alarmEditMode) {
            auto ret = AppWindow::onInput(inputEvent);
            if (ret == false) {
                // alarm setting finished
                alarmEditMode = false;
                setFocusItem(nullptr);
                alarmSetSpinner->setEditMode(EditMode::Browse);
                alarmStatus = BellAlarm::Status::ACTIVATED;
                alarmSetSpinner->setAlarmStatus(alarmStatus);
                return true;
            }
            return ret;
        }
        else {
            LOG_INFO("Open MainMenu");
            application->switchWindow(gui::window::name::bell_main_menu, nullptr);
            return true;
        }
    }

    void BellMainWindow::handleEditModeEntry()
    {
        if (!alarmEditMode) {
            alarmEditMode = true;
            setFocusItem(alarmSetSpinner);
            alarmSetSpinner->setEditMode(EditMode::Edit);
        }
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
