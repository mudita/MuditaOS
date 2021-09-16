// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmPopup.hpp"
#include "data/AlarmPopupParams.hpp"

#include <InputEvent.hpp>
#include <i18n/i18n.hpp>

#include <ImageBox.hpp>
#include <TextFixedSize.hpp>
#include <ImageBoxWithText.hpp>

using namespace style::popup::alarm;

namespace gui
{

    AlarmPopup::AlarmPopup(app::ApplicationCommon *app, const std::string &name) : AppWindow(app, name)
    {
        buildInterface();
    }

    void AlarmPopup::buildInterface()
    {
        AppWindow::buildInterface();

        addArcOverlay();
        createMainLayout();
    }

    void AlarmPopup::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        /// Use AlarmPopupType to determine popup origin

        if (auto params = dynamic_cast<AlarmPopupParams *>(data); params && mode == ShowMode::GUI_SHOW_INIT) {

            bottomBar->setActive(BottomBar::Side::RIGHT, true);
            bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::stop));

            addAlarmLabels(params->getAlarmTimeString());

            /// Apply snooze button visibility based on Event records (snooze interval != 0);

            bottomBar->setActive(BottomBar::Side::CENTER, true);
            bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));

            addSnoozeLabel(params->getSnoozeTimeString());

            auto snoozeIcon = new ImageBoxWithText(
                body, new Image("snooze_icon", ImageTypeSpecifier::W_G), utils::translate("app_popup_snooze_text"));
            snoozeIcon->activatedCallback = [=](gui::Item &item) {
                LOG_DEBUG("TODO: Add SNOOZE action");
                return true;
            };

            setFocusItem(snoozeIcon);
            body->resizeItems();
        }
    }

    status_bar::Configuration AlarmPopup::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        appConfiguration.enable(status_bar::Indicator::Time);
        appConfiguration.enable(status_bar::Indicator::PhoneMode);
        appConfiguration.enable(status_bar::Indicator::Battery);
        appConfiguration.enable(status_bar::Indicator::Signal);
        appConfiguration.enable(status_bar::Indicator::SimCard);
        appConfiguration.enable(status_bar::Indicator::Bluetooth);

        return appConfiguration;
    }

    bool AlarmPopup::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease() && inputEvent.is(KeyCode::KEY_RF)) {
            LOG_DEBUG("TODO: Add STOP action");
            return true;
        }

        if (AppWindow::onInput(inputEvent)) {
            return true;
        }
        return false;
    }

    void AlarmPopup::addArcOverlay()
    {
        auto arcOverlay = new gui::ImageBox(this,
                                            style::window::default_left_margin,
                                            style::popup::alarm::window::y,
                                            style::window::default_body_width,
                                            style::window::default_body_width,
                                            new Image(ImageCircle::name, ImageTypeSpecifier::W_G));
        arcOverlay->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        arcOverlay->setEdges(RectangleEdge::None);
    }

    void AlarmPopup::createMainLayout()
    {
        body = new VBox(this,
                        style::window::default_left_margin,
                        style::popup::alarm::window::y,
                        style::window::default_body_width,
                        style::window::default_body_width);

        body->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        body->setEdges(RectangleEdge::None);
    }

    void AlarmPopup::addAlarmLabels(std::string timeString)
    {
        auto alarmTimeLabel = new gui::TextFixedSize(body);
        alarmTimeLabel->drawUnderline(false);
        alarmTimeLabel->setFont(style::window::font::largelight);
        alarmTimeLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        alarmTimeLabel->setText(timeString);
        alarmTimeLabel->setMinimumWidthToFitText(alarmTimeLabel->getText());
        alarmTimeLabel->setMinimumHeightToFitText();
        alarmTimeLabel->setMargins(Margins(
            0, style::popup::alarm::AlarmTimeLabel::top_margin, 0, style::popup::alarm::AlarmTimeLabel::bottom_margin));

        auto alarmLabel = new gui::TextFixedSize(body);
        alarmLabel->drawUnderline(false);
        alarmLabel->setFont(style::window::font::mediumlight);
        alarmLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        alarmLabel->setText(utils::translate("app_popup_alarm_text"));
        alarmLabel->setMinimumWidthToFitText(alarmLabel->getText());
        alarmLabel->setMinimumHeightToFitText();
    }

    void AlarmPopup::addSnoozeLabel(std::string timeString)
    {
        auto snoozeLabelBox = new VBox(body);
        snoozeLabelBox->setMinimumSize(style::window::default_body_width, style::popup::alarm::SnoozeLabel::h);
        snoozeLabelBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        snoozeLabelBox->setMargins(Margins(0, style::popup::alarm::SnoozeLabel::top_margin, 0, 0));
        snoozeLabelBox->setEdges(RectangleEdge::None);

        if (!timeString.empty()) {
            auto snoozeLabel = new gui::TextFixedSize(snoozeLabelBox);
            snoozeLabel->drawUnderline(false);
            snoozeLabel->setFont(style::window::font::mediumlight);
            snoozeLabel->setAlignment(
                gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));
            snoozeLabel->setText(utils::translate("app_popup_alarm_snoozed_till") + std::string{" "} + timeString);
            snoozeLabel->setMinimumWidthToFitText(snoozeLabel->getText());
            snoozeLabel->setMinimumHeightToFitText();
        }
    }
} // namespace gui
