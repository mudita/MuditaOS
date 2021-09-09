// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellMainSetHour.hpp"
#include <data/BellMainStyle.hpp>

#include <i18n/i18n.hpp>
#include <log/log.hpp>
#include <gui/input/InputEvent.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>

namespace gui
{
    BellMainSetHour::BellMainSetHour(app::ApplicationCommon *app)
        : AppWindow(app, gui::window::name::bell_main_set_hour)
    {
        buildInterface();
    }

    void BellMainSetHour::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        auto vBox = new gui::VBox(this,
                                  style::window::default_left_margin,
                                  style::window::default_vertical_pos,
                                  style::window::default_body_width,
                                  style::window::default_body_height);
        vBox->setEdges(gui::RectangleEdge::None);

        spinnerHour = new gui::Spinner(0, 23, 1, gui::Boundaries::Continuous);
        spinnerHour->setMinimumSize(bellMainStyle::clockSpinner::default_width,
                                    bellMainStyle::clockSpinner::default_height);

        spinnerHour->setFixedFieldWidth(bellMainStyle::clockSpinner::hour_or_munte_field_width);
        spinnerHour->setFont(style::window::font::supersizemelight);
        spinnerHour->setFocus(true);
        spinnerHour->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));

        auto colonLabel = new gui::Label();
        colonLabel->setFont(style::window::font::verybigbold);
        colonLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        colonLabel->setMinimumSize(20, 100);
        colonLabel->setEdges(gui::RectangleEdge::None);

        spinnerMinute = new gui::Spinner(0, 59, 1, gui::Boundaries::Continuous);
        spinnerMinute->setMinimumSize(bellMainStyle::clockSpinner::default_width,
                                      bellMainStyle::clockSpinner::default_height);
        spinnerMinute->setFixedFieldWidth(bellMainStyle::clockSpinner::hour_or_munte_field_width);
        spinnerMinute->setFont(style::window::font::supersizemelight);
        spinnerMinute->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));

        auto hBox = new gui::HBox();
        hBox->setEdges(gui::RectangleEdge::None);
        hBox->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        hBox->setMinimumSize(style::window::default_body_width, 200);
        hBox->addWidget(spinnerHour);
        hBox->addWidget(colonLabel);
        hBox->addWidget(spinnerMinute);
        vBox->addWidget(hBox);
        spinnerHour->setCurrentValue(10);
        colonLabel->setText(":");
        spinnerMinute->setCurrentValue(15);
        setFocusItem(spinnerHour);
    }

    bool BellMainSetHour::onInput(const InputEvent &inputEvent)
    {
        LOG_INFO("Bell Main Window Input");
        if (inputEvent.isShortRelease()) {
            switch (inputEvent.getKeyCode()) {
            case KeyCode::KEY_RF:
                application->returnToPreviousWindow();
                break;
            case KeyCode::KEY_LEFT:
            case KeyCode::KEY_RIGHT: {
                if ((spinnerHour != nullptr) && (spinnerMinute != nullptr)) {
                    if (focusItem == spinnerHour) {
                        setFocusItem(spinnerMinute);
                    }
                    else {
                        setFocusItem(spinnerHour);
                    }
                    return true;
                }
                break;
            }
            default:
                break;
            }
        }

        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    status_bar::Configuration BellMainSetHour::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        appConfiguration.disable(gui::status_bar::Indicator::Time);
        return appConfiguration;
    }

} // namespace gui
