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

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_select"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));
        bottomBar->setText(gui::BottomBar::Side::CENTER, "Message");

        // create circles to hold images inside
        for (uint32_t i = 0; i < 2; ++i) {
            rects[i] = new gui::Rect(this, 0, 0, 80, 80);
            rects[i]->setFilled(false);
            rects[i]->setRadius(40);
            rects[i]->setPenFocusWidth(3);
            rects[i]->setPenWidth(1);
        }

        rects[static_cast<uint32_t>(FocusRects::Speaker)]->setPosition(150, 400);
        rects[static_cast<uint32_t>(FocusRects::Micrphone)]->setPosition(250, 400);

        imageSpeaker[static_cast<uint32_t>(AudioState::ON)] =
            new gui::Image(rects[static_cast<uint32_t>(FocusRects::Speaker)], 20, 20, 0, 0, "speaker_on");
        imageSpeaker[static_cast<uint32_t>(AudioState::OFF)] =
            new gui::Image(rects[static_cast<uint32_t>(FocusRects::Speaker)], 20, 20, 0, 0, "speaker_off");
        imageSpeaker[static_cast<uint32_t>(AudioState::ON)]->setVisible(false);
        imageSpeaker[static_cast<uint32_t>(AudioState::OFF)]->setVisible(false);

        sendSmsIcon                       = new gui::SendSmsIcon(this, sendMessageIcon::x, sendMessageIcon::y);
        sendSmsIcon->focusChangedCallback = [=](gui::Item &item) {
            LOG_INFO("Send message gets focus");
            bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get("app_call_message"));
            return true;
        };
        sendSmsIcon->activatedCallback = [=](gui::Item &item) {
            LOG_ERROR("TODO: Reject call and send message template");
            return true;
        };

        imageMicrophone[static_cast<uint32_t>(AudioState::ON)] =
            new gui::Image(rects[static_cast<uint32_t>(FocusRects::Micrphone)], 20, 20, 0, 0, "microphone_on");
        imageMicrophone[static_cast<uint32_t>(AudioState::OFF)] =
            new gui::Image(rects[static_cast<uint32_t>(FocusRects::Micrphone)], 20, 20, 0, 0, "microphone_off");
        imageMicrophone[static_cast<uint32_t>(AudioState::ON)]->setVisible(false);
        imageMicrophone[static_cast<uint32_t>(AudioState::OFF)]->setVisible(false);

        // top circle image
        imageCircleTop    = new gui::Image(this, 116, 82, 0, 0, "circle_top");
        imageCircleBottom = new gui::Image(this, 106, 266, 0, 0, "circle_bottom");

        durationLabel = new gui::Label(this, 0, 220, 480, 80);
        durationLabel->setFilled(false);
        durationLabel->setBorderColor(gui::ColorNoColor);
        durationLabel->setFont(style::window::font::largelight);
        durationLabel->setAlignement(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

        numberLabel = new gui::Label(this, 11, 150, 480 - 22, 80);
        numberLabel->setFilled(false);
        numberLabel->setBorderColor(gui::ColorNoColor);
        numberLabel->setFont(style::window::font::bigbold);
        numberLabel->setAlignement(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

        // define navigation between labels
        rects[static_cast<uint32_t>(FocusRects::Speaker)]->setNavigationItem(
            NavigationDirection::LEFT, rects[static_cast<uint32_t>(FocusRects::Micrphone)]);
        rects[static_cast<uint32_t>(FocusRects::Speaker)]->setNavigationItem(
            NavigationDirection::RIGHT, rects[static_cast<uint32_t>(FocusRects::Micrphone)]);

        rects[static_cast<uint32_t>(FocusRects::Micrphone)]->setNavigationItem(
            NavigationDirection::LEFT, rects[static_cast<uint32_t>(FocusRects::Speaker)]);
        rects[static_cast<uint32_t>(FocusRects::Micrphone)]->setNavigationItem(
            NavigationDirection::RIGHT, rects[static_cast<uint32_t>(FocusRects::Speaker)]);

        // focus callbacks
        rects[static_cast<uint32_t>(FocusRects::Speaker)]->focusChangedCallback = [=](gui::Item &item) {
            LOG_INFO("Speaker gets focus");
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_speaker"));
            return true;
        };

        rects[static_cast<uint32_t>(FocusRects::Micrphone)]->focusChangedCallback = [=](gui::Item &item) {
            LOG_INFO("Mute gets focus");
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_mute"));
            return true;
        };

        // activation callbacks
        rects[static_cast<uint32_t>(FocusRects::Speaker)]->activatedCallback = [=](gui::Item &item) {
            LOG_INFO("Speaker activated");
            // update icon
            imageSpeaker[static_cast<uint32_t>(speakerState)]->setVisible(false);
            speakerState = (speakerState == AudioState::ON) ? AudioState::OFF : AudioState::ON;
            imageSpeaker[static_cast<uint32_t>(speakerState)]->setVisible(true);

            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

            (speakerState == AudioState::ON) ? AudioServiceAPI::RoutingSpeakerPhone(this->application, true)
                                             : AudioServiceAPI::RoutingSpeakerPhone(this->application, false);

            return true;
        };

        rects[static_cast<uint32_t>(FocusRects::Micrphone)]->activatedCallback = [=](gui::Item &item) {
            LOG_INFO("Mute activated");

            // update icon
            imageMicrophone[static_cast<uint32_t>(microphoneState)]->setVisible(false);
            microphoneState = (microphoneState == AudioState::ON) ? AudioState::OFF : AudioState::ON;
            imageMicrophone[static_cast<uint32_t>(microphoneState)]->setVisible(true);

            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

            (microphoneState == AudioState::ON) ? AudioServiceAPI::RoutingMute(this->application, false)
                                                : AudioServiceAPI::RoutingMute(this->application, true);

            return true;
        };
    }

    void CallWindow::destroyInterface()
    {
        AppWindow::destroyInterface();

        if (numberLabel) {
            removeWidget(numberLabel);
            delete numberLabel;
            numberLabel = nullptr;
        }
        if (durationLabel) {
            removeWidget(durationLabel);
            delete durationLabel;
            durationLabel = nullptr;
        }
        if (rects[0]) {
            removeWidget(rects[0]);
            delete rects[0];
            rects[0] = nullptr;
        }
        if (rects[1]) {
            removeWidget(rects[1]);
            delete rects[1];
            rects[1] = nullptr;
        }

        removeWidget(sendSmsIcon);
        delete sendSmsIcon;
        sendSmsIcon = nullptr;

        if (imageCircleTop) {
            removeWidget(imageCircleTop);
            delete imageCircleTop;
            imageCircleTop = nullptr;
        }
        if (imageCircleBottom) {
            removeWidget(imageCircleBottom);
            delete imageCircleBottom;
            imageCircleBottom = nullptr;
        }

        children.clear();
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

        sendSmsIcon->setVisible(false);
        rects[static_cast<uint32_t>(FocusRects::Speaker)]->setVisible(false);
        rects[static_cast<uint32_t>(FocusRects::Micrphone)]->setVisible(false);
        durationLabel->setVisible(false);
        setFocusItem(nullptr);

        auto showIconsLambda = [=]() {
            rects[static_cast<uint32_t>(FocusRects::Speaker)]->setVisible(true);
            rects[static_cast<uint32_t>(FocusRects::Micrphone)]->setVisible(true);

            imageSpeaker[static_cast<uint32_t>(speakerState)]->setVisible(true);
            imageMicrophone[static_cast<uint32_t>(microphoneState)]->setVisible(true);

            setFocusItem(rects[static_cast<uint32_t>(FocusRects::Speaker)]);
        };

        // show state of the window
        switch (state) {
        case State::INCOMING_CALL: {
            bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
            bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
            bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
            bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get("app_call_answer"));
            bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get("app_call_reject"));
            durationLabel->setText(utils::localize.get("app_call_is_calling"));
            durationLabel->setVisible(true);
            sendSmsIcon->setVisible(true);
            setFocusItem(sendSmsIcon);
        } break;
        case State::CALL_ENDED: {
            bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
            bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
            bottomBar->setActive(gui::BottomBar::Side::RIGHT, false);

            durationLabel->setVisible(true);
            durationLabel->setText(utils::localize.get("app_call_call_ended"));
        } break;
        case State::CALL_IN_PROGRESS: {
            //			titleLabel->setText("CALL_IN_PROGRESS");
            durationLabel->setVisible(true);

            bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
            bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
            bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
            bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get("app_call_end_call"));

            showIconsLambda();
        } break;
        case State::IDLE: {
            //			titleLabel->setText("IDLE");
        } break;
        case State::OUTGOING_CALL: {
            //			titleLabel->setText("OUTGOING_CALL");

            bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
            bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
            bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
            bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get("app_call_end_call"));

            showIconsLambda();
            durationLabel->setText(utils::localize.get("app_call_calling"));
            durationLabel->setVisible(true);
        } break;
        };
    }

    void CallWindow::setCallNumber(std::string)
    {}

    void CallWindow::updateDuration(time_t duration)
    {
        uint32_t seconds = 0;
        uint32_t minutes = 0;
        uint32_t hours   = 0;
        uint32_t days    = 0;

        days = duration / 86400;
        duration -= days * 86400;
        hours = duration / 3600;
        duration -= hours * 3600;
        minutes = duration / 60;
        duration -= minutes * 60;
        seconds = duration;

        std::stringstream ss;
        if (days)
            ss << days << ":";
        if (hours)
            ss << hours << ":";
        if (hours && minutes < 10)
            ss << "0";
        ss << minutes << ":";
        ss << std::setfill('0') << std::setw(2) << seconds;

        durationLabel->setText(ss.str());
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
            LOG_INFO("number = %s recognized as contact id = %u, name = %s",
                     phoneNumber.c_str(),
                     rec.dbID,
                     rec.getFormattedName().c_str());
            phoneNumber = rec.getFormattedName();
        }
        else if (records->size() > 1) {
            LOG_ERROR("number = %s recognized as more than one contact", phoneNumber.c_str());
            for (auto i : *records) {
                LOG_ERROR("contact id = %u, name = %s", i.dbID, i.getFormattedName().c_str());
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

        LOG_INFO("key code: %d, state: %d",
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
