// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallWindow.hpp"
#include <application-call/data/CallWindowData.hpp>

#include "GuiTimer.hpp"
#include "InputEvent.hpp"
#include "application-call/widgets/Icons.hpp"
#include "log/log.hpp"
#include "service-appmgr/Controller.hpp"

#include "application-call/ApplicationCall.hpp"
#include "application-call/data/CallSwitchData.hpp"

#include "i18/i18.hpp"

#include "service-audio/api/AudioServiceAPI.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"
#include "service-db/api/DBServiceAPI.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "application-call/data/CallAppStyle.hpp"
#include "time/time_conversion.hpp"

#include <UiCommonActions.hpp>
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
    using namespace call;

    CallWindow::CallWindow(app::Application *app, std::string windowName) : AppWindow(app, windowName)
    {
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

        topBar->setActive(gui::TopBar::Elements::BATTERY, true);
        topBar->setActive(gui::TopBar::Elements::SIGNAL, true);
        topBar->setActive(gui::TopBar::Elements::TIME, true);

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
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::Switch));
            return true;
        };
        speakerIcon->activatedCallback = [=](gui::Item &item) {
            speakerIcon->setNext();
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            LOG_INFO("Speaker activated %d", static_cast<int>(speakerIcon->get()));

            switch (speakerIcon->get()) {
            case SpeakerIconState::SPEAKER: {
                AudioServiceAPI::SendEvent(this->application, audio::EventType::CallSpeakerphoneOff);
            } break;
            case SpeakerIconState::SPEAKERON: {
                AudioServiceAPI::SendEvent(this->application, audio::EventType::CallSpeakerphoneOn);
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
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::Switch));
            return true;
        };
        microphoneIcon->activatedCallback = [=](gui::Item &item) {
            microphoneIcon->setNext();
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            LOG_INFO("Mic activated activated %d", static_cast<int>(microphoneIcon->get()));

            microphoneIcon->get() == MicrophoneIconState::MUTED
                ? AudioServiceAPI::SendEvent(this->application, audio::EventType::CallMute)
                : AudioServiceAPI::SendEvent(this->application, audio::EventType::CallUnmute);

            return true;
        };

        sendSmsIcon                       = new gui::SendSmsIcon(this, sendMessageIcon::x, sendMessageIcon::y);
        sendSmsIcon->focusChangedCallback = [=](gui::Item &item) {
            LOG_INFO("Send message gets focus");
            bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(strings::message));
            return true;
        };
        sendSmsIcon->activatedCallback = [=](gui::Item &item) {
            LOG_INFO("Send messaage template and reject the call");
            app::sms(application, app::SmsOperation::Template, phoneNumber);
            return true;
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

    CallWindow::~CallWindow()
    {}

    void CallWindow::setState(State state)
    {
        LOG_INFO("==> Call state change: %s -> %s", c_str(this->state), c_str(state));
        this->state = state;
        setVisibleState();
    }

    const State &CallWindow::getState()
    {
        return state;
    }

    void CallWindow::setVisibleState()
    {
        // show state of the window
        switch (state) {
        case State::INCOMING_CALL: {
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
            stopCallTimer();

            bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
            bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
            bottomBar->setActive(gui::BottomBar::Side::RIGHT, false);
            durationLabel->setVisible(true);
            durationLabel->setText(utils::localize.get(strings::callended));
            sendSmsIcon->setVisible(false);
            speakerIcon->setVisible(false);
            microphoneIcon->setVisible(false);
            setFocusItem(nullptr);
            speakerIcon->set(SpeakerIconState::SPEAKER);
            microphoneIcon->set(MicrophoneIconState::MUTE);

            connectTimerOnExit();
        } break;
        case State::CALL_IN_PROGRESS: {
            bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
            bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
            bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(strings::endcall), true);
            durationLabel->setVisible(true);
            sendSmsIcon->setVisible(false);
            speakerIcon->setVisible(true);
            microphoneIcon->setVisible(true);
            setFocusItem(getFocusItem() != sendSmsIcon ? getFocusItem() : microphoneIcon);
        } break;
        case State::OUTGOING_CALL: {
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

    void CallWindow::setCallNumber(std::string)
    {}

    void CallWindow::updateDuration(const utils::time::Duration duration)
    {
        if (durationLabel != nullptr) {
            durationLabel->setText(duration.str());
        }
    }

    bool CallWindow::handleSwitchData(SwitchData *data)
    {
        if (data == nullptr) {
            LOG_DEBUG("Received null pointer");
            return false;
        }

        if (dynamic_cast<SMSTemplateSent *>(data) != nullptr) {
            CellularServiceAPI::HangupCall(application);
            return true;
        }

        if (data->getDescription() == app::CallSwitchData::descriptionStr) {
            auto *callData = dynamic_cast<app::CallSwitchData *>(data);
            assert(callData != nullptr);
            phoneNumber = callData->getPhoneNumber();
            switch (callData->getType()) {
            case app::CallSwitchData::Type::INCOMING_CALL: {
                state = State::INCOMING_CALL;
            } break;

            case app::CallSwitchData::Type::EXECUTE_CALL: {
                state = State::OUTGOING_CALL;
            } break;

            default:
                LOG_ERROR("Unhandled type");
            }
        }
        else {
            return false;
        }

        auto contact            = DBServiceAPI::MatchContactByPhoneNumber(this->application, phoneNumber);
        std::string displayName = phoneNumber.getFormatted();
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

        setVisibleState();
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

        return true;
    }

    void CallWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        auto app = static_cast<app::ApplicationCall *>(application);

        if (dynamic_cast<app::IncomingCallData *>(data) != nullptr) {
            if (getState() == call::State::INCOMING_CALL) {
                LOG_INFO("ignoring call incoming");
            }
            app->startRinging();
            setState(call::State::INCOMING_CALL);
            return;
        }

        if (dynamic_cast<app::CallAbortData *>(data) != nullptr) {
            app->stopAudio();
            setState(State::CALL_ENDED);
            return;
        }

        if (dynamic_cast<app::CallActiveData *>(data) != nullptr) {
            app->startRouting();
            runCallTimer();
            return;
        }

        if (dynamic_cast<app::ExecuteCallData *>(data) != nullptr) {
            AudioServiceAPI::RoutingStart(application);
            runCallTimer();
            setState(State::OUTGOING_CALL);
            return;
        }
    }

    bool CallWindow::handleLeftButton()
    {
        if (state == State::INCOMING_CALL) {
            auto ret = CellularServiceAPI::AnswerIncomingCall(application);

            LOG_INFO("AnswerIncomingCall: %s", (ret ? "OK" : "FAIL"));
            return true;
        }

        return false;
    }

    bool CallWindow::handleRightButton()
    {
        switch (state) {
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
        auto app = dynamic_cast<app::ApplicationCall *>(application);
        assert(app != nullptr);
        app->transmitDtmfTone(digit);
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
        auto app = dynamic_cast<app::ApplicationCall *>(application);
        assert(app != nullptr);

        auto timer = std::make_unique<app::GuiTimer>(app);
        timer->setInterval(app->getDelayedStopTime());

        timerCallback = [this](Item &, Timer &timer) {
            setState(State::IDLE);
            detachTimer(timer);
            app::manager::Controller::switchBack(application);
            return true;
        };
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
            updateDuration(utils::time::Duration(callDuration, 0));
            callDuration++;
            application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            return true;
        };
        timer->start();
        application->connect(std::move(timer), this);
    }

    void CallWindow::stopCallTimer()
    {
        callDuration = 0;
        stop_timer = true;
    }
} /* namespace gui */
