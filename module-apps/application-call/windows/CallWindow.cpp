// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationCall.hpp"
#include "CallAppStyle.hpp"
#include "CallState.hpp"
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
    using AudioEvent = app::CallWindowInterface::AudioEvent;

    CallWindow::CallWindow(app::ApplicationCommon *app, app::CallWindowInterface *interface, std::string windowName)
        : AppWindow(app, windowName), interface(interface)
    {
        assert(interface != nullptr);
        assert(app != nullptr);
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

            microphoneIcon->get() == MicrophoneIconState::MUTED ? interface->sendAudioEvent(AudioEvent::Mute)
                                                                : interface->sendAudioEvent(AudioEvent::Unmute);

            return true;
        };

        speakerIcon                    = new SpeakerIcon(iconsBox);
        speakerIcon->activatedCallback = [=](gui::Item &item) {
            speakerIcon->setNext();
            LOG_INFO("Speaker activated %d", static_cast<int>(speakerIcon->get()));

            switch (speakerIcon->get()) {
            case SpeakerIconState::SPEAKER: {
                interface->sendAudioEvent(AudioEvent::LoudspeakerOff);
            } break;
            case SpeakerIconState::SPEAKERON: {
                interface->sendAudioEvent(AudioEvent::LoudspeakerOn);
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
            constexpr auto preventAutoLock  = true;
            auto msg                        = std::make_unique<SMSSendTemplateRequest>(phoneNumber, preventAutoLock);
            msg->ignoreCurrentWindowOnStack = true;
            return app::manager::Controller::sendAction(application,
                                                        app::manager::actions::ShowSmsTemplates,
                                                        std::move(msg),
                                                        app::manager::OnSwitchBehaviour::RunInBackground);
        };

        // define navigation between icons
        microphoneIcon->setNavigationItem(NavigationDirection::LEFT, speakerIcon);
        microphoneIcon->setNavigationItem(NavigationDirection::RIGHT, speakerIcon);

        speakerIcon->setNavigationItem(NavigationDirection::LEFT, microphoneIcon);
        speakerIcon->setNavigationItem(NavigationDirection::RIGHT, microphoneIcon);

        setState(State::IDLE);
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

    void CallWindow::setState(State state)
    {
        auto prevState = getState();
        LOG_INFO("==> Call state change: %s -> %s", c_str(prevState), c_str(state));
        interface->setCallState(state);

        switch (state) {
        case State::INCOMING_CALL: {
            navBar->setText(gui::nav_bar::Side::Left, utils::translate(strings::answer), true);
            navBar->setText(gui::nav_bar::Side::Right, utils::translate(strings::reject), true);
            durationLabel->setText(utils::translate(strings::iscalling));
            durationLabel->setVisible(true);
            speakerIcon->setVisible(false);
            microphoneIcon->setVisible(false);
            if (phoneNumber.getFormatted().empty()) {
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
        } break;
        case State::CALL_ENDED: {
            interface->stopAudio();
            stopCallTimer();
            navBar->setActive(gui::nav_bar::Side::Left, false);
            navBar->setActive(gui::nav_bar::Side::Center, false);
            navBar->setActive(gui::nav_bar::Side::Right, false);
            durationLabel->setVisible(true);
            setCallEndMessage();
            sendSmsIcon->setVisible(false);
            speakerIcon->setVisible(false);
            microphoneIcon->setVisible(false);
            speakerIcon->set(SpeakerIconState::SPEAKER);
            microphoneIcon->set(MicrophoneIconState::MUTE);
            setFocusItem(nullptr);
            connectTimerOnExit();
        } break;
        case State::CALL_IN_PROGRESS: {
            if (prevState == State::INCOMING_CALL) { // otherwise it is already started
                interface->startAudioRouting();
            }
            runCallTimer();
            navBar->setActive(gui::nav_bar::Side::Left, false);
            navBar->setActive(gui::nav_bar::Side::Center, false);
            navBar->setText(gui::nav_bar::Side::Right, utils::translate(strings::endcall), true);
            navBar->setText(gui::nav_bar::Side::Center, utils::translate(style::strings::common::Switch), true);
            durationLabel->setVisible(true);
            sendSmsIcon->setVisible(false);
            speakerIcon->setVisible(true);
            microphoneIcon->setVisible(true);
            setFocusItem(microphoneIcon);
        } break;
        case State::OUTGOING_CALL: {
            interface->startAudioRouting();
            navBar->setActive(gui::nav_bar::Side::Left, false);
            navBar->setActive(gui::nav_bar::Side::Center, false);
            navBar->setText(gui::nav_bar::Side::Right, utils::translate(strings::endcall), true);
            navBar->setText(gui::nav_bar::Side::Center, utils::translate(style::strings::common::Switch), true);
            durationLabel->setText(utils::translate(strings::calling));
            durationLabel->setVisible(true);
            sendSmsIcon->setVisible(false);
            speakerIcon->setVisible(true);
            microphoneIcon->setVisible(true);
            setFocusItem(microphoneIcon);
        } break;
        case State::IDLE:
            stopCallTimer();
            [[fallthrough]];
        default:
            numberLabel->clear();
            navBar->setActive(gui::nav_bar::Side::Left, false);
            navBar->setActive(gui::nav_bar::Side::Center, false);
            navBar->setActive(gui::nav_bar::Side::Right, false);
            durationLabel->setVisible(false);
            sendSmsIcon->setVisible(false);
            speakerIcon->setVisible(false);
            microphoneIcon->setVisible(false);
            setFocusItem(nullptr);
            break;
        };
        iconsBox->resizeItems();
    }

    auto CallWindow::getState() const noexcept -> State
    {
        return interface->getCallState();
    }

    void CallWindow::updateDuration(const utils::time::Duration duration)
    {
        if (durationLabel != nullptr) {
            durationLabel->setText(duration.str());
        }
    }

    void CallWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto callData = dynamic_cast<app::CallSwitchData *>(data); callData != nullptr) {
            phoneNumber = callData->getPhoneNumber();
            if (!callData->getPhoneNumber().getFormatted().empty()) {
                auto contact     = DBServiceAPI::MatchContactByPhoneNumber(this->application, phoneNumber);
                auto displayName = phoneNumber.getFormatted();
                if (contact && !contact->isTemporary()) {
                    LOG_INFO("number recognized as contact id = %" PRIu32, contact->ID);
                    displayName = contact->getFormattedName();
                }
                else {
                    LOG_INFO("number was not recognized as any valid contact");
                }

                numberLabel->setText(displayName);
            }
            else {
                numberLabel->setText(utils::translate(strings::privateNumber));
            }

            if (dynamic_cast<app::IncomingCallData *>(data) != nullptr) {
                if (getState() == State::INCOMING_CALL) {
                    LOG_DEBUG("ignoring IncomingCallData message");
                    return;
                }
                callEndType = CallEndType::None;
                setState(State::INCOMING_CALL);
                return;
            }
            if (dynamic_cast<app::ExecuteCallData *>(data) != nullptr) {
                callEndType = CallEndType::None;
                setState(State::OUTGOING_CALL);
                return;
            }
        }

        if (dynamic_cast<app::CallAbortData *>(data) != nullptr) {
            if (callEndType == CallEndType::None) {
                callEndType = CallEndType::Ended;
            }
            setState(State::CALL_ENDED);
            return;
        }

        if (dynamic_cast<app::CallActiveData *>(data) != nullptr) {
            if (getState() != State::CALL_ENDED) {
                setState(State::CALL_IN_PROGRESS);
                return;
            }
            LOG_DEBUG("Ignoring CallActiveData message");
            return;
        }

        if (dynamic_cast<SMSTemplateSent *>(data) != nullptr) {
            callEndType = CallEndType::Rejected;
            interface->hangupCall();
            return;
        }
    }

    bool CallWindow::handleLeftButton()
    {
        if (getState() == State::INCOMING_CALL) {
            interface->answerIncomingCall();
            return true;
        }

        return false;
    }

    bool CallWindow::handleRightButton()
    {
        switch (getState()) {
        case State::INCOMING_CALL:
            callEndType = CallEndType::Rejected;
            interface->hangupCall();
            return true;
        case State::OUTGOING_CALL:
        case State::CALL_IN_PROGRESS:
            callEndType = CallEndType::Ended;
            interface->hangupCall();
            return true;
        case State::IDLE:
        case State::CALL_ENDED:
            break;
        }

        return false;
    }

    bool CallWindow::handleHeadsetOkButton()
    {
        switch (getState()) {
        case State::INCOMING_CALL:
            interface->answerIncomingCall();
            return true;
        case State::OUTGOING_CALL:
            [[fallthrough]];
        case State::CALL_IN_PROGRESS:
            interface->hangupCall();
            return true;
        case State::IDLE:
        case State::CALL_ENDED:
            break;
        }

        return false;
    }

    bool CallWindow::handleDigit(const uint32_t digit)
    {
        interface->transmitDtmfTone(digit);
        return true;
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
                handled = handleLeftButton();
                break;
            case KeyCode::KEY_RF:
                handled = handleRightButton();
                break;
            case KeyCode::HEADSET_OK:
                handled = handleHeadsetOkButton();
                break;
            default:
                break;
            }
            if (!handled && code != 0) {
                handled = handleDigit(code);
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
            setState(State::IDLE);
            app::manager::Controller::switchBack(application);
            application->popCurrentWindow();
            return true;
        };
        delayedExitTimer =
            app::GuiTimerFactory::createSingleShotTimer(application, this, "DelayedExitTimer", getDelayedStopTime());
        delayedExitTimer.start();
    }

    void CallWindow::runCallTimer()
    {
        constexpr auto callTimeTimeout = std::chrono::seconds{1};
        callTimer = app::GuiTimerFactory::createPeriodicTimer(application, durationLabel, "CallTime", callTimeTimeout);
        durationLabel->timerCallback = [&](Item &item, sys::Timer &timer) {
            std::chrono::time_point<std::chrono::system_clock> systemUnitDuration(callDuration);
            updateDuration(std::chrono::system_clock::to_time_t(systemUnitDuration));
            callDuration++;
            LOG_DEBUG("Update duration timer callback - %" PRIu32, static_cast<uint32_t>(callDuration.count()));
            application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            return true;
        };
        callTimer.start();
    }

    void CallWindow::stopCallTimer()
    {
        callDuration = std::chrono::seconds().zero();
        if (callTimer.isActive()) {
            callTimer.stop();
        }
    }

    void CallWindow::setCallEndMessage()
    {
        switch (callEndType) {
        case CallEndType::None:
        case CallEndType::Ended:
            durationLabel->setText(utils::translate(strings::callended));
            break;
        case CallEndType::Rejected:
            durationLabel->setText(utils::translate(strings::callrejected));
            break;
        }
    }
} /* namespace gui */
