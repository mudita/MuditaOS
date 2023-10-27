// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallPresenter.hpp"
#include <application-call/data/CallAppStyle.hpp>

namespace app::call
{

    CallWindowContract::Presenter::Presenter(std::shared_ptr<app::call::AbstractCallModel> callModel) : model(callModel)
    {}

    void CallWindowContract::Presenter::Presenter::attachCallbacks()
    {
        model->attachCallStateChangeCallback([this]() { this->handleCallStateChange(); });
        model->attachDurationChangeCallback([this]() { this->handleCallDurationChange(); });
    }

    CallWindowContract::Presenter::~Presenter() noexcept
    {
        model->clear();
    }

    void CallWindowContract::Presenter::handleCallDurationChange()
    {
        if (not isCallInProgress()) {
            return;
        }
        auto view = getView();
        view->updateDuration(utils::time::Duration(model->getTime()).str());
        view->refreshWindow();
    }

    void CallWindowContract::Presenter::handleCallStateChange()
    {
        buildLayout();
        auto view = getView();
        view->refreshWindow();
    }

    void CallWindowContract::Presenter::buildLayout()
    {
        auto view      = getView();
        auto callState = model->getState();

        view->setEnteredNumberVisible(false);

        switch (callState) {
        case app::call::CallState::Incoming: {
            view->updateDuration(utils::translate(callAppStyle::strings::iscalling));
            view->setIncomingCallLayout(model->getCallerId().empty());
            view->updateNumber(getCallerId());
            break;
        }
        case app::call::CallState::Outgoing: {
            view->updateDuration(utils::translate(callAppStyle::strings::calling));
            view->updateNumber(getCallerId());
            view->setActiveCallLayout();
            break;
        }
        case app::call::CallState::Active: {
            view->updateDuration(utils::time::Duration(model->getTime()).str());
            view->setEnteredNumberVisible(true);
            view->setActiveCallLayout();
            break;
        }
        case app::call::CallState::Rejected: {
            view->updateDuration(utils::translate(callAppStyle::strings::callrejected));
            view->setCallEndedLayout();
            break;
        }
        case app::call::CallState::Ended: {
            view->updateDuration(utils::translate(callAppStyle::strings::callended));
            view->setCallEndedLayout();
            break;
        }
        case app::call::CallState::Missed: {
            model->setState(CallState::None);
            break;
        }
        case app::call::CallState::Disconnecting: {
            view->updateDuration(utils::translate(callAppStyle::strings::endingcall));
            view->setCallEndedLayout(false);
            break;
        }
        case app::call::CallState::None: {
            view->clearNavBar();
            break;
        }
        }
    }

    UTF8 CallWindowContract::Presenter::getCallerId()
    {
        auto callerId = model->getCallerId();
        if (not callerId.empty()) {
            return callerId;
        }
        return UTF8(utils::translate(callAppStyle::strings::privateNumber));
    }

    bool CallWindowContract::Presenter::handleLeftButton()
    {
        if (model->getState() == app::call::CallState::Incoming) {
            model->answerCall();
            return true;
        }
        return false;
    }

    bool CallWindowContract::Presenter::handleRightButton()
    {
        if (isIncomingCall() || isCallInProgress()) {
            model->hangUpCall();
            return true;
        }
        return false;
    }

    bool CallWindowContract::Presenter::handleHeadsetOk()
    {
        if (isIncomingCall()) {
            model->answerCall();
            return true;
        }

        if (isCallInProgress()) {
            model->hangUpCall();
            return true;
        }

        return false;
    }

    void CallWindowContract::Presenter::hangUpCall()
    {
        model->hangUpCall();
        model->setState(app::call::CallState::Disconnecting);
    }

    void CallWindowContract::Presenter::sendSms(const UTF8 &smsBody)
    {
        if (not model->sendSms(smsBody)) {
            LOG_ERROR("SMS sending failed!");
        }
    }

    bool CallWindowContract::Presenter::handleDigitButton(const uint32_t &digit)
    {
        auto digitChar = static_cast<char>(digit);
        if (isCallInProgress() && isProvidedCharValid(digitChar)) {
            getView()->updateEnteredNumber(digitChar);
        }
        model->transmitDtmfTone(digit);
        return true;
    }

    void CallWindowContract::Presenter::muteCall()
    {
        model->muteCall();
    }

    void CallWindowContract::Presenter::unmuteCall()
    {
        model->unmuteCall();
    }

    void CallWindowContract::Presenter::turnLoudspeakerOn()
    {
        model->turnLoudspeakerOn();
    }

    void CallWindowContract::Presenter::turnLoudspeakerOff()
    {
        model->turnLoudspeakerOff();
    }

    bool CallWindowContract::Presenter::isIncomingCall()
    {
        return model->getState() == CallState::Incoming;
    }

    bool CallWindowContract::Presenter::isCallInProgress()
    {
        if (auto state = model->getState(); state == CallState::Outgoing || state == CallState::Active) {
            return true;
        }
        return false;
    }

    utils::PhoneNumber CallWindowContract::Presenter::getPhoneNumber()
    {
        return model->getPhoneNumber();
    }

    void CallWindowContract::Presenter::handleDelayedViewClose()
    {
        model->setState(CallState::None);
    }

    void CallWindowContract::Presenter::processCurrentRouting(const audio::Profile::Type &routingType)
    {
        if (routingType == audio::Profile::Type::RoutingLoudspeaker) {
            getView()->setSpeakerIconState(gui::SpeakerIconState::SPEAKERON);
        }
        else {
            getView()->setSpeakerIconState(gui::SpeakerIconState::SPEAKER);
        }
    }
    void CallWindowContract::Presenter::clearModel()
    {
        model->clear();
    }
    bool CallWindowContract::Presenter::isProvidedCharValid(const char digitChar)
    {
        // Return true if the character comes from the phone's numeric keypad
        return (std::isdigit(digitChar) || digitChar == '*' || digitChar == '#');
    }

} // namespace app::call
