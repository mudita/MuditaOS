﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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

#include <service-audio/AudioServiceAPI.hpp>
#include <service-cellular/CellularServiceAPI.hpp>
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

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(strings::message));

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
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::Switch), false);
            return true;
        };
        speakerIcon->activatedCallback = [=](gui::Item &item) {
            speakerIcon->setNext();
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            LOG_INFO("Speaker activated %d", static_cast<int>(speakerIcon->get()));

            switch (speakerIcon->get()) {
            case SpeakerIconState::SPEAKER: {
                AudioServiceAPI::SendEvent(this->application, audio::EventType::CallLoudspeakerOff);
            } break;
            case SpeakerIconState::SPEAKERON: {
                AudioServiceAPI::SendEvent(this->application, audio::EventType::CallLoudspeakerOn);
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
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::Switch), false);
            return true;
        };
        microphoneIcon->activatedCallback = [=](gui::Item &item) {
            microphoneIcon->setNext();
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            LOG_INFO("Mic activated %d", static_cast<int>(microphoneIcon->get()));

            microphoneIcon->get() == MicrophoneIconState::MUTED
                ? AudioServiceAPI::SendEvent(this->application, audio::EventType::CallMute)
                : AudioServiceAPI::SendEvent(this->application, audio::EventType::CallUnmute);

            return true;
        };

        sendSmsIcon                       = new gui::SendSmsIcon(this, sendMessageIcon::x, sendMessageIcon::y);
        sendSmsIcon->focusChangedCallback = [=](gui::Item &item) {
            LOG_DEBUG("Send message get/lost focus");
            bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(strings::message), false);
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

    void CallWindow::setState(State state)
    {
        auto prevState = getState();
        LOG_INFO("==> Call state change: %s -> %s", c_str(prevState), c_str(state));
        interface->setState(state);

        switch (state) {
        case State::INCOMING_CALL: {
            interface->startRinging();
            bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
            bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get(strings::answer), true);
            bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(strings::reject), true);
            durationLabel->setText(utils::localize.get(strings::iscalling));
            durationLabel->setVisible(true);
            sendSmsIcon->setVisible(true);
            speakerIcon->setVisible(false);
            microphoneIcon->setVisible(false);
            setFocusItem(sendSmsIcon);
        } break;
        case State::CALL_ENDED: {
            interface->stopAudio();
            stopCallTimer();
            bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
            bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
            bottomBar->setActive(gui::BottomBar::Side::RIGHT, false);
            durationLabel->setVisible(true);
            durationLabel->setText(utils::localize.get(strings::callended));
            sendSmsIcon->setVisible(false);
            speakerIcon->setVisible(false);
            microphoneIcon->setVisible(false);
            speakerIcon->set(SpeakerIconState::SPEAKER);
            microphoneIcon->set(MicrophoneIconState::MUTE);
            setFocusItem(nullptr);
            connectTimerOnExit();
            LOG_FATAL("CALL_ENDED");
        } break;
        case State::CALL_IN_PROGRESS: {
            if (prevState == State::INCOMING_CALL) { // otherwise it is already started
                interface->startRouting();
            }
            runCallTimer();
            bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
            bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
            bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(strings::endcall), true);
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
            interface->startRouting();
            bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
            bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
            bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(strings::endcall), true);
            durationLabel->setText(utils::localize.get(strings::calling));
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
            phoneNumber      = callData->getPhoneNumber();
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

            if (dynamic_cast<app::IncomingCallData *>(data) != nullptr) {
                if (getState() == State::INCOMING_CALL) {
                    LOG_DEBUG("ignoring call incoming");
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
            setState(State::CALL_IN_PROGRESS);
            return;
        }

        if (dynamic_cast<SMSTemplateSent *>(data) != nullptr) {
            CellularServiceAPI::HangupCall(application);
            return;
        }
    }

    bool CallWindow::handleLeftButton()
    {
        if (getState() == State::INCOMING_CALL) {
            auto ret = CellularServiceAPI::AnswerIncomingCall(application);

            LOG_INFO("AnswerIncomingCall: %s", (ret ? "OK" : "FAIL"));
            return true;
        }

        return false;
    }

    bool CallWindow::handleRightButton()
    {
        switch (getState()) {
        case State::INCOMING_CALL:
        case State::OUTGOING_CALL:
        case State::CALL_IN_PROGRESS:
            CellularServiceAPI::HangupCall(application);
            return true;
            break;
        default:
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
                 static_cast<uint32_t>(inputEvent.keyCode),
                 static_cast<uint32_t>(inputEvent.state));

        bool handled = false;

        // process only if key is released
        // InputEvent::State::keyReleasedLong is necessary for KeyCode::KEY_RF to properly abort the active call
        if (inputEvent.state == InputEvent::State::keyReleasedShort ||
            inputEvent.state == InputEvent::State::keyReleasedLong) {
            auto code = translator.handle(inputEvent.key, InputMode({InputMode::phone}).get());
            switch (inputEvent.keyCode) {
            case KeyCode::KEY_LF:
                handled = handleLeftButton();
                break;
            case KeyCode::KEY_RF:
                handled = handleRightButton();
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
        auto timer = std::make_unique<app::GuiTimer>(application);
        timer->setInterval(getDelayedStopTime());

        timerCallback = [this](Item &, Timer &timer) {
            LOG_DEBUG("Delayed exit timer callback");
            setState(State::IDLE);
            detachTimer(timer);
            app::manager::Controller::switchBack(application);
            return true;
        };
        timer->start();
        application->connect(std::move(timer), this);
    }

    void CallWindow::runCallTimer()
    {
        static const sys::ms one_second = 1000;
        stop_timer                      = false;
        auto timer    = std::make_unique<app::GuiTimer>("CallTime", application, one_second, Timer::Continous);
        timerCallback = [&](Item &, Timer &timer) {
            if (stop_timer) {
                timer.stop();
                detachTimer(timer);
            }
            std::chrono::time_point<std::chrono::system_clock> systemUnitDuration(callDuration);
            updateDuration(std::chrono::system_clock::to_time_t(systemUnitDuration));
            callDuration++;
            LOG_DEBUG("Update duration timer callback - %" PRIu32, static_cast<uint32_t>(callDuration.count()));
            application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            return true;
        };
        timer->start();
        application->connect(std::move(timer), this);
    }

    void CallWindow::stopCallTimer()
    {
        callDuration = std::chrono::seconds().zero();
        stop_timer   = true;
    }
} /* namespace gui */
