/*
 * @file CallWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 12 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "CallWindow.hpp"
#include <memory>
#include <functional>
#include <sstream>
#include <iomanip>

#include "application-call/widgets/Icons.hpp"
#include "service-appmgr/ApplicationManager.hpp"

#include "application-call/ApplicationCall.hpp"
#include "application-call/data/CallSwitchData.hpp"

#include "i18/i18.hpp"

#include "service-audio/api/AudioServiceAPI.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"
#include "service-db/api/DBServiceAPI.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "application-call/data/CallAppStyle.hpp"

namespace gui
{
    using namespace callAppStyle;
    using namespace callAppStyle::callWindow;

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
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

        numberLabel = new gui::Label(this, numberLabel::x, numberLabel::y, numberLabel::w, numberLabel::h);
        numberLabel->setFilled(false);
        numberLabel->setBorderColor(gui::ColorNoColor);
        numberLabel->setFont(style::window::font::largelight);
        numberLabel->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_TOP));

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
                AudioServiceAPI::RoutingSpeakerPhone(this->application, false);
            } break;
            case SpeakerIconState::SPEAKERON: {
                AudioServiceAPI::RoutingSpeakerPhone(this->application, true);
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

            microphoneIcon->get() == MicrophoneIconState::MUTED ? AudioServiceAPI::RoutingMute(this->application, false)
                                                                : AudioServiceAPI::RoutingMute(this->application, true);

            return true;
        };

        sendSmsIcon                       = new gui::SendSmsIcon(this, sendMessageIcon::x, sendMessageIcon::y);
        sendSmsIcon->focusChangedCallback = [=](gui::Item &item) {
            LOG_INFO("Send message gets focus");
            bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(strings::message));
            return true;
        };
        sendSmsIcon->activatedCallback = [=](gui::Item &item) {
            LOG_ERROR("TODO: Reject call and send message template");
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
        this->state = state;
        setVisibleState();
    }

    const CallWindow::State &CallWindow::getState()
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

    void CallWindow::updateDuration(const utils::time::Duration &duration)
    {
        durationLabel->setText(duration.str());
    }

    bool CallWindow::handleSwitchData(SwitchData *data)
    {

        if (data == nullptr) {
            LOG_ERROR("Received null pointer");
            return false;
        }

        std::string phoneNumber;
        if (data->getDescription() == app::CallSwitchData::descriptionStr) {
            app::CallSwitchData *callData = reinterpret_cast<app::CallSwitchData *>(data);
            phoneNumber                   = callData->getPhoneNumber();
            switch (callData->getType()) {
            case app::CallSwitchData::Type::INCOMMING_CALL: {
                state = State::INCOMING_CALL;
            } break;

            case app::CallSwitchData::Type::EXECUTE_CALL: {
                state = State::OUTGOING_CALL;
            } break;

            default:
                LOG_ERROR("Unhandled type");
                phoneNumber = "";
            }
        }
        else {
            LOG_ERROR("Unrecognized SwitchData");
            return false;
        }

        auto records = DBServiceAPI::ContactGetByPhoneNumber(this->application, phoneNumber);
        if (records->size() == 1) {
            auto rec = records->operator[](0);
            LOG_INFO("number = %s recognized as contact id = %" PRIu32 ", name = %s",
                     phoneNumber.c_str(),
                     rec.ID,
                     rec.getFormattedName().c_str());
            phoneNumber = rec.getFormattedName();
        }
        else if (records->size() > 1) {
            LOG_ERROR("number = %s recognized as more than one contact", phoneNumber.c_str());
            for (auto i : *records) {
                LOG_ERROR("contact id = %" PRIu32 ", name = %s", i.ID, i.getFormattedName().c_str());
            }
        }
        else {
            LOG_INFO("number = %s was not recognized as any valid contact", phoneNumber.c_str());
        }

        numberLabel->setText(phoneNumber);

        setVisibleState();
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

        return true;
    }

    void CallWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

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
        }

        if (handled) {
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            return true;
        }
        else {
            return AppWindow::onInput(inputEvent);
        }
    }

} /* namespace gui */
