// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallWindow.hpp"

#include "GuiTimer.hpp"
#include "InputEvent.hpp"
#include "application-call/data/CallState.hpp"
#include "application-call/widgets/StateIcons.hpp"
#include "log/log.hpp"
#include "service-appmgr/Controller.hpp"

#include "application-call/ApplicationCall.hpp"
#include "application-call/data/CallSwitchData.hpp"

#include <i18n/i18n.hpp>

#include "service-db/DBServiceAPI.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "application-call/data/CallAppStyle.hpp"
#include "time/time_conversion.hpp"

#include <application-messages/data/SMSdata.hpp>
#include <InputMode.hpp>

#include <memory>
#include <functional>
#include <sstream>
#include <iomanip>

#include <cassert>
#include <magic_enum.hpp>

namespace gui
{
    using namespace callAppStyle;
    using namespace callAppStyle::callWindow;
    using namespace app::call;
    using AudioEvent = app::CallWindowInterface::AudioEvent;

    CallWindow::CallWindow(app::Application *app, app::CallWindowInterface *interface, std::string windowName)
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

        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);

        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(strings::message));

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

        speakerIcon                       = new SpeakerIcon(this, speakerIcon::x, speakerIcon::y);
        speakerIcon->focusChangedCallback = [=](gui::Item &item) {
            LOG_DEBUG("speakerIcon get/lost focus");
            bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::Switch), false);
            return true;
        };
        speakerIcon->activatedCallback = [=](gui::Item &item) {
            speakerIcon->setNext();
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
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

        microphoneIcon                       = new MicrophoneIcon(this, microphoneIcon::x, microphoneIcon::y);
        microphoneIcon->focusChangedCallback = [=](gui::Item &item) {
            LOG_DEBUG("microphoneIcon get/lost focus");
            bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::Switch), false);
            return true;
        };
        microphoneIcon->activatedCallback = [=](gui::Item &item) {
            microphoneIcon->setNext();
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            LOG_INFO("Mic activated %d", static_cast<int>(microphoneIcon->get()));

            microphoneIcon->get() == MicrophoneIconState::MUTED ? interface->sendAudioEvent(AudioEvent::Mute)
                                                                : interface->sendAudioEvent(AudioEvent::Unmute);

            return true;
        };

        sendSmsIcon                       = new gui::SendSmsIcon(this, sendMessageIcon::x, sendMessageIcon::y);
        sendSmsIcon->focusChangedCallback = [=](gui::Item &item) {
            LOG_DEBUG("Send message get/lost focus");
            bottomBar->setText(
                gui::BottomBar::Side::CENTER, utils::translate(style::strings::common::send), item.focus);
            return true;
        };
        sendSmsIcon->activatedCallback = [=](gui::Item &item) {
            LOG_INFO("Send message template and reject the call");
            return app::manager::Controller::sendAction(application,
                                                        app::manager::actions::ShowSmsTemplates,
                                                        std::make_unique<SMSSendTemplateRequest>(phoneNumber));
        };

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

    top_bar::Configuration CallWindow::configureTopBar(top_bar::Configuration appConfiguration)
    {
        appConfiguration.enable(top_bar::Indicator::NetworkAccessTechnology);
        return appConfiguration;
    }

    void CallWindow::setState(State state)
    {
        auto prevState = getState();
        LOG_INFO("==> Call state change: %s -> %s", c_str(prevState), c_str(state));
        interface->setState(state);

        switch (state) {
        case State::INCOMING_CALL: {
            interface->startAudioRinging();
            bottomBar->setText(gui::BottomBar::Side::LEFT, utils::translate(strings::answer), true);
            bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(strings::reject), true);
            durationLabel->setText(utils::translate(strings::iscalling));
            durationLabel->setVisible(true);
            speakerIcon->setVisible(false);
            microphoneIcon->setVisible(false);
            if (phoneNumber.getFormatted().empty()) {
                bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
                sendSmsIcon->setVisible(false);
                setFocusItem(nullptr);
            }
            else {
                bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
                sendSmsIcon->setVisible(true);
                setFocusItem(sendSmsIcon);
            }
        } break;
        case State::CALL_ENDED: {
            interface->stopAudio();
            stopCallTimer();
            bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
            bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
            bottomBar->setActive(gui::BottomBar::Side::RIGHT, false);
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
            bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
            bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
            bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(strings::endcall), true);
            durationLabel->setVisible(true);
            sendSmsIcon->setVisible(false);
            speakerIcon->setVisible(true);
            microphoneIcon->setVisible(true);
            auto focusItem = getFocusItem();
            if (focusItem != microphoneIcon || focusItem != speakerIcon) {
                setFocusItem(microphoneIcon);
            }
        } break;
        case State::OUTGOING_CALL: {
            interface->startAudioRouting();
            bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
            bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
            bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(strings::endcall), true);
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
            bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
            bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
            bottomBar->setActive(gui::BottomBar::Side::RIGHT, false);
            durationLabel->setVisible(false);
            sendSmsIcon->setVisible(false);
            speakerIcon->setVisible(false);
            microphoneIcon->setVisible(false);
            setFocusItem(nullptr);
            break;
        };
    }

    auto CallWindow::getState() const noexcept -> State
    {
        return interface->getState();
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
                if (contact) {
                    LOG_INFO("number = %s recognized as contact id = %" PRIu32 ", name = %s",
                             phoneNumber.getEntered().c_str(),
                             contact->ID,
                             contact->getFormattedName().c_str());
                    displayName = contact->getFormattedName();
                }
                else {
                    LOG_INFO("number = %s was not recognized as any valid contact", phoneNumber.getEntered().c_str());
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
                setState(State::INCOMING_CALL);
                return;
            }
            if (dynamic_cast<app::ExecuteCallData *>(data) != nullptr) {
                setState(State::OUTGOING_CALL);
                return;
            }
        }

        if (dynamic_cast<app::CallAbortData *>(data) != nullptr) {
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
        LOG_INFO("key code: %" PRIu32 ", state: %" PRIu32,
                 static_cast<uint32_t>(inputEvent.getKeyCode()),
                 static_cast<uint32_t>(inputEvent.getState()));

        bool handled = false;

        // process only if key is released
        // InputEvent::State::keyReleasedLong is necessary for KeyCode::KEY_RF to properly abort the active call
        if (inputEvent.isKeyRelease()) {
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
        case CallEndType::Ended:
            durationLabel->setText(utils::translate(strings::callended));
            break;
        case CallEndType::Rejected:
            durationLabel->setText(utils::translate(strings::callrejected));
            break;
        case CallEndType::None:
            break;
        }
    }
} /* namespace gui */
