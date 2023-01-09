// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationCall.hpp"
#include "CallAppStyle.hpp"
#include "CallSwitchData.hpp"
#include "CallWindow.hpp"
#include "StateIcons.hpp"

#include <application-messages/data/SMSdata.hpp>
#include <GuiTimer.hpp>
#include <i18n/i18n.hpp>
#include <InputEvent.hpp>
#include <text/modes/InputMode.hpp>
#include <Label.hpp>
#include <log/log.hpp>
#include <magic_enum.hpp>
#include <Margins.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <time/time_conversion.hpp>

#include <cassert>
#include <functional>
#include <iomanip>
#include <memory>
#include <sstream>

namespace gui
{
    using namespace callAppStyle;
    using namespace callAppStyle::callWindow;
    using namespace app::call;

    CallWindow::CallWindow(app::ApplicationCommon *app,
                           std::unique_ptr<app::call::CallWindowContract::Presenter> &&windowPresenter)
        : gui::AppWindow{app, app::window::name_call}, presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void CallWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }
    void CallWindow::buildInterface()
    {
        AppWindow::buildInterface();

        navBar->setActive(nav_bar::Side::Center, true);
        navBar->setActive(nav_bar::Side::Right, true);

        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::select));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
        navBar->setText(gui::nav_bar::Side::Center, utils::translate(strings::message));

        // top circle image
        imageCircleTop = new gui::Image(this, imageCircleTop::x, imageCircleTop::y, 0, 0, imageCircleTop::name);
        imageCircleBottom =
            new gui::Image(this, imageCircleBottom::x, imageCircleBottom::y, 0, 0, imageCircleBottom::name);

        durationLabel = new gui::Label(this, durationLabel::x, durationLabel::y, durationLabel::w, durationLabel::h);
        durationLabel->setFilled(false);
        durationLabel->setBorderColor(gui::ColorNoColor);
        durationLabel->setFont(style::window::font::mediumlight);
        durationLabel->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));

        numberLabel = new gui::Label(this, numberLabel::x, numberLabel::y, numberLabel::w, numberLabel::h);
        numberLabel->setFilled(false);
        numberLabel->setBorderColor(gui::ColorNoColor);
        numberLabel->setFont(style::window::font::largelight);
        numberLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));

        iconsBox = new HBox(
            this, style::window::default_left_margin, iconsBox::y, style::window::default_body_width, iconsBox::h);
        iconsBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        iconsBox->setEdges(RectangleEdge::None);

        microphoneIcon                    = new MicrophoneIcon(iconsBox);
        microphoneIcon->activatedCallback = [=](gui::Item &item) {
            microphoneIcon->setNext();
            LOG_INFO("Mic activated %d", static_cast<int>(microphoneIcon->get()));

            microphoneIcon->get() == MicrophoneIconState::MUTED ? presenter->muteCall() : presenter->unmuteCall();

            return true;
        };

        speakerIcon                    = new SpeakerIcon(iconsBox);
        speakerIcon->activatedCallback = [=](gui::Item &item) {
            speakerIcon->setNext();
            LOG_INFO("Speaker activated %d", static_cast<int>(speakerIcon->get()));

            switch (speakerIcon->get()) {
            case SpeakerIconState::SPEAKER: {
                presenter->turnLoudspeakerOff();
            } break;
            case SpeakerIconState::SPEAKERON: {
                presenter->turnLoudspeakerOn();
            } break;
            // case SpeakerIconState::BLUETOOTH: {
            //     // TODO: need implementation
            // } break;
            default:
                break;
            }

            return true;
        };

        sendSmsIcon                    = new gui::SendSmsIcon(iconsBox);
        sendSmsIcon->activatedCallback = [=](gui::Item &item) {
            LOG_INFO("Send message template and reject the call");
            constexpr auto preventAutoLock = true;
            auto msg = std::make_unique<SMSSendTemplateRequest>(presenter->getPhoneNumber().getView(), preventAutoLock);
            msg->ignoreCurrentWindowOnStack = true;
            return app::manager::Controller::sendAction(application,
                                                        app::manager::actions::ShowSmsTemplates,
                                                        std::move(msg),
                                                        app::manager::OnSwitchBehaviour::RunInBackground);
        };

        sendSmsIcon->setVisible(false);
        microphoneIcon->setVisible(false);
        speakerIcon->setVisible(false);

        // define navigation between icons
        microphoneIcon->setNavigationItem(NavigationDirection::LEFT, speakerIcon);
        microphoneIcon->setNavigationItem(NavigationDirection::RIGHT, speakerIcon);

        speakerIcon->setNavigationItem(NavigationDirection::LEFT, microphoneIcon);
        speakerIcon->setNavigationItem(NavigationDirection::RIGHT, microphoneIcon);
    }

    void CallWindow::destroyInterface()
    {
        erase();
    }

    status_bar::Configuration CallWindow::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        appConfiguration.enable(status_bar::Indicator::NetworkAccessTechnology);
        return appConfiguration;
    }

    void CallWindow::updateDuration(const UTF8 &text, bool isVisible)
    {
        if (durationLabel != nullptr) {
            durationLabel->setText(text);
            durationLabel->setVisible(isVisible);
        }
    }

    void CallWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        presenter->buildLayout();

        if (auto switchData = dynamic_cast<SMSTemplateSent *>(data); switchData != nullptr) {
            presenter->hangUpCall();
            presenter->sendSms(switchData->getText());
            return;
        }
    }

    bool CallWindow::onInput(const InputEvent &inputEvent)
    {
        bool handled = false;

        // process only if key is released
        // InputEvent::State::keyReleasedLong is necessary for KeyCode::KEY_RF to properly abort the active call
        if (inputEvent.isKeyRelease()) {
            LOG_INFO("key released");
            auto code = translator.handle(inputEvent.getRawKey(), InputMode({InputMode::phone}).get());
            switch (inputEvent.getKeyCode()) {
            case KeyCode::KEY_LF:
                handled = presenter->handleLeftButton();
                break;
            case KeyCode::KEY_RF:
                handled = presenter->handleRightButton();
                break;
            case KeyCode::HEADSET_OK:
                handled = presenter->handleHeadsetOk();
                break;
            default:
                break;
            }
            if (!handled && code != 0) {
                handled = presenter->handleDigitButton(code);
            }
        }

        if (handled) {
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            return true;
        }
        else {
            return AppWindow::onInput(inputEvent);
        }
    }

    void CallWindow::connectTimerOnExit()
    {
        timerCallback = [this](Item &, sys::Timer &timer) {
            LOG_DEBUG("Delayed exit timer callback");
            presenter->handleDelayedViewClose();
            application->popCurrentWindow();
            app::manager::Controller::switchBack(application);
            return true;
        };
        delayedExitTimer =
            app::GuiTimerFactory::createSingleShotTimer(application, this, "DelayedExitTimer", getDelayedStopTime());
        delayedExitTimer.start();
    }

    void CallWindow::refreshWindow()
    {
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    void CallWindow::setNavBarForActiveCall()
    {
        navBar->setActive(gui::nav_bar::Side::Left, false);
        navBar->setActive(gui::nav_bar::Side::Center, false);
        navBar->setText(gui::nav_bar::Side::Right, utils::translate(strings::endcall), true);
        navBar->setText(gui::nav_bar::Side::Center, utils::translate(style::strings::common::Switch), true);
    }

    void CallWindow::setNavBarForIncomingCall()
    {
        navBar->setText(gui::nav_bar::Side::Left, utils::translate(strings::answer), true);
        navBar->setText(gui::nav_bar::Side::Right, utils::translate(strings::reject), true);
    }

    void CallWindow::clearNavBar()
    {
        navBar->setActive(gui::nav_bar::Side::Left, false);
        navBar->setActive(gui::nav_bar::Side::Center, false);
        navBar->setActive(gui::nav_bar::Side::Right, false);
    }
    void CallWindow::setIncomingCallLayout(bool isValidCallerId)
    {
        navBar->setText(gui::nav_bar::Side::Left, utils::translate(strings::answer), true);
        navBar->setText(gui::nav_bar::Side::Right, utils::translate(strings::reject), true);

        microphoneIcon->setVisible(false);
        speakerIcon->setVisible(false);

        if (isValidCallerId) {
            navBar->setActive(gui::nav_bar::Side::Center, false);
            sendSmsIcon->setVisible(false);
            setFocusItem(nullptr);
        }
        else {
            navBar->setActive(gui::nav_bar::Side::Center, true);
            navBar->setText(gui::nav_bar::Side::Center, utils::translate(style::strings::common::send), true);
            sendSmsIcon->setVisible(true);
            setFocusItem(sendSmsIcon);
        }

        iconsBox->resizeItems();
    }

    void CallWindow::setActiveCallLayout()
    {
        navBar->setActive(gui::nav_bar::Side::Left, false);
        navBar->setActive(gui::nav_bar::Side::Center, false);
        navBar->setText(gui::nav_bar::Side::Right, utils::translate(strings::endcall), true);
        navBar->setText(gui::nav_bar::Side::Center, utils::translate(style::strings::common::Switch), true);

        sendSmsIcon->setVisible(false);
        microphoneIcon->setVisible(true);
        speakerIcon->setVisible(true);

        iconsBox->resizeItems();

        setFocusItem(microphoneIcon);
    }

    void CallWindow::setCallEndedLayout(bool delayedClose)
    {
        navBar->setActive(gui::nav_bar::Side::Left, false);
        navBar->setActive(gui::nav_bar::Side::Center, false);
        navBar->setActive(gui::nav_bar::Side::Right, false);

        sendSmsIcon->setVisible(false);
        microphoneIcon->setVisible(false);
        speakerIcon->setVisible(false);

        iconsBox->resizeItems();

        setFocusItem(nullptr);
        if (delayedClose) {
            connectTimerOnExit();
        }
    }

    void CallWindow::updateNumber(const UTF8 &text)
    {
        numberLabel->setText(text);
    }

} /* namespace gui */
