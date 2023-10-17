// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmPopup.hpp"
#include "log/log.hpp"
#include "time/dateCommon.hpp"

#include <popups/data/AlarmPopupRequestParams.hpp>
#include <InputEvent.hpp>
#include <i18n/i18n.hpp>

#include <ImageBox.hpp>

using namespace style::popup::alarm;

namespace gui
{
    AlarmPopup::AlarmPopup(app::ApplicationCommon *app,
                           const std::string &name,
                           std::shared_ptr<app::popup::AlarmPopupPresenter> presenter)
        : AppWindow(app, name), app::popup::AlarmPopupContract::View(std::move(presenter))
    {
        buildInterface();
        getPresenter()->setRefreshWindowCallback([this]() {
            updateItems();
            refillText();
        });
    }

    void AlarmPopup::buildInterface()
    {
        AppWindow::buildInterface();

        addArcOverlay();
        createMainLayout();
        createLabels();
        createSnoozeButton();
    }

    void AlarmPopup::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if ((data != nullptr) && mode == ShowMode::GUI_SHOW_INIT) {
            auto params = dynamic_cast<AlarmPopupRequestParams *>(data);
            assert(params);

            const auto popupType = params->getPopupType();
            getPresenter()->resetModel();
            if (popupType == AlarmPopupType::AlarmRing) {
                getPresenter()->setModel(params->popRecord());
                refillText();
            }
            else if (popupType == AlarmPopupType::SnoozeCheck) {
                getPresenter()->setSnoozedRecord(params->popSnoozed());
                getPresenter()->processIfSnoozed();
            }
            updateItems();
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
        if (inputEvent.isShortRelease(KeyCode::KEY_RF) || inputEvent.isLongRelease(KeyCode::KEY_RF)) {
            getPresenter()->stopAlarm();
            return true;
        }
        else if (inputEvent.isShortRelease(KeyCode::KEY_LF) && getPresenter()->haveSnoozedSkip()) {
            getPresenter()->skipToNextSnooze();
            return true;
        }

        return AppWindow::onInput(inputEvent);
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

        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::stop));
        navBar->setActive(nav_bar::Side::Right, true);

        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::select));
        navBar->setActive(nav_bar::Side::Center, false);

        navBar->setText(nav_bar::Side::Left, utils::translate(style::strings::common::skip));
        navBar->setActive(nav_bar::Side::Left, false);
    }

    void AlarmPopup::createLabels()
    {
        alarmTimeLabel = new gui::TextFixedSize(body);
        alarmTimeLabel->drawUnderline(false);
        alarmTimeLabel->setFont(style::window::font::largelight);
        alarmTimeLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        alarmTimeLabel->setMaximumWidth(style::window::default_body_width);
        alarmTimeLabel->setMinimumHeightToFitText();
        alarmTimeLabel->setMargins(Margins(
            0, style::popup::alarm::AlarmTimeLabel::top_margin, 0, style::popup::alarm::AlarmTimeLabel::bottom_margin));

        alarmLabel = new gui::TextFixedSize(body);
        alarmLabel->drawUnderline(false);
        alarmLabel->setFont(style::window::font::mediumlight);
        alarmLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        alarmLabel->setText(utils::translate("app_popup_alarm_text"));
        alarmLabel->setMinimumWidthToFitText(alarmLabel->getText());
        alarmLabel->setMinimumHeightToFitText();

        snoozeLabelBox = new VBox(body);
        snoozeLabelBox->setMinimumSize(style::window::default_body_width, style::popup::alarm::SnoozeLabel::h);
        snoozeLabelBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        snoozeLabelBox->setMargins(Margins(0, style::popup::alarm::SnoozeLabel::top_margin, 0, 0));
        snoozeLabelBox->setEdges(RectangleEdge::None);

        snoozeLabel = new gui::TextFixedSize(snoozeLabelBox);
        snoozeLabel->drawUnderline(false);
        snoozeLabel->setFont(style::window::font::mediumlight);
        snoozeLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));
        snoozeLabel->setMaximumWidth(style::window::default_body_width);
        snoozeLabel->setMinimumHeightToFitText();
    }
    void AlarmPopup::createSnoozeButton()
    {
        snoozeIcon = new ImageBoxWithText(
            body, new Image("snooze_icon", ImageTypeSpecifier::W_G), utils::translate("app_popup_snooze_text"));

        snoozeIcon->activatedCallback = [=](gui::Item & /*item*/) {
            getPresenter()->snoozeHit();
            return true;
        };

        setFocusItem(snoozeIcon);
    }

    void AlarmPopup::updateItems()
    {
        snoozeLabel->setVisible(getPresenter()->haveSnoozedSkip());
        snoozeIcon->setVisible(getPresenter()->isSnoozeAble());
        navBar->setActive(nav_bar::Side::Left, getPresenter()->haveSnoozedSkip());
        navBar->setActive(nav_bar::Side::Center, getPresenter()->isSnoozeAble());
        body->resizeItems();
    }

    void AlarmPopup::refillText()
    {
        if (alarmTimeLabel) {
            alarmTimeLabel->setText(getPresenter()->startedAt());
        }
        if (snoozeLabel) {
            snoozeLabel->setText(utils::translate("app_popup_alarm_snoozed_till") + std::string{" "} +
                                 getPresenter()->snoozedTill());
        }
        body->resizeItems();
    }
} // namespace gui
