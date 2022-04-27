// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallModel.hpp"

#include <service-cellular/service-cellular/CellularServiceAPI.hpp>
#include <service-evtmgr/service-evtmgr/EVMessages.hpp>
#include <service-evtmgr/Constants.hpp>

namespace app::call
{
    time_t CallModel::getTime()
    {
        return callTime;
    }

    void CallModel::setTime(time_t newTime)
    {
        callTime = newTime;

        if (notifyPresenterOnDurationChange != nullptr) {
            notifyPresenterOnDurationChange();
        }
    }

    CallState CallModel::getState()
    {
        return callState;
    }

    void CallModel::setState(CallState newState)
    {
        if (callState == newState) {
            LOG_INFO("Dropping call state change");
        }

        if (callState == CallState::Incoming && newState == CallState::Ended && callWasRejected) {
            callWasRejected = false;
            callState       = CallState::Rejected;
        }
        else {
            callState = newState;
        }

        if (callState == CallState::Active) {
            turnOnKeypadBacklight();
        }
        else {
            turnOffKeypadBacklight();
        }

        if (notifyPresenterOnCallStateChange != nullptr) {
            notifyPresenterOnCallStateChange();
        }
    }

    void CallModel::hangUpCall()
    {
        CellularServiceAPI::HangupCall(application);
        if (callState == CallState::Incoming) {
            callWasRejected = true;
        }
    }

    void CallModel::answerCall()
    {
        CellularServiceAPI::AnswerIncomingCall(application);
    }

    bool CallModel::sendSms(const UTF8 &smsBody)
    {
        if (phoneNumber.getView().getEntered().empty() || smsBody.length() == 0) {
            LOG_WARN("Number or sms body is empty");
            return false;
        }
        SMSRecord record;
        record.number = phoneNumber.getView();
        record.body   = smsBody;
        record.type   = SMSType::QUEUED;
        record.date   = std::time(nullptr);

        using db::query::SMSAdd;
        const auto [succeed, _] =
            DBServiceAPI::GetQuery(application, db::Interface::Name::SMS, std::make_unique<SMSAdd>(record));
        return succeed;
    }

    void CallModel::transmitDtmfTone(const uint32_t &digit)
    {
        CellularServiceAPI::TransmitDtmfTones(application, digit);
    }

    utils::PhoneNumber CallModel::getPhoneNumber()
    {
        return phoneNumber;
    }

    void CallModel::muteCall()
    {
        CellularServiceAPI::CallAudioMuteEvent(application);
    }

    void CallModel::unmuteCall()
    {
        CellularServiceAPI::CallAudioUnmuteEvent(application);
    }

    void CallModel::turnLoudspeakerOn()
    {
        CellularServiceAPI::CallAudioLoudspeakerOnEvent(application);
    }

    void CallModel::turnLoudspeakerOff()
    {
        CellularServiceAPI::CallAudioLoudspeakerOffEvent(application);
    }

    void CallModel::turnOnKeypadBacklight()
    {
        application->bus.sendUnicast(
            std::make_shared<sevm::KeypadBacklightMessage>(bsp::keypad_backlight::Action::turnOnCallMode),
            service::name::evt_manager);
    }

    void CallModel::turnOffKeypadBacklight()
    {
        application->bus.sendUnicast(
            std::make_shared<sevm::KeypadBacklightMessage>(bsp::keypad_backlight::Action::turnOffCallMode),
            service::name::evt_manager);
    }

    void CallModel::clear()
    {
        callState = CallState::None;
        callTime  = 0;
        callerId.clear();
        callWasRejected = false;
        phoneNumber     = utils::PhoneNumber();

        notifyPresenterOnDurationChange  = nullptr;
        notifyPresenterOnCallStateChange = nullptr;
    }
    void CallModel::attachDurationChangeCallback(std::function<void()> callback)
    {
        notifyPresenterOnDurationChange = callback;
    }

    void CallModel::attachCallStateChangeCallback(std::function<void()> callback)
    {
        notifyPresenterOnCallStateChange = callback;
    }

    void CallModel::setPhoneNumber(const utils::PhoneNumber number)
    {
        if (phoneNumber.getFormatted() == number.getFormatted()) {
            LOG_INFO("Dropping number duplication.");
            return;
        }

        phoneNumber = number;

        if (notifyPresenterOnCallStateChange != nullptr) {
            notifyPresenterOnCallStateChange();
        }
    }

    UTF8 CallModel::getCallerId()
    {
        if (phoneNumber.getFormatted().empty()) {
            LOG_INFO("No number provided");
            callerId = UTF8();
            return callerId;
        }

        auto contact = DBServiceAPI::MatchContactByPhoneNumber(application, phoneNumber.getView());
        if (contact && !contact->isTemporary()) {
            LOG_INFO("number recognized as contact id = %" PRIu32, contact->ID);
            callerId = contact->getFormattedName();
        }
        else {
            LOG_INFO("number was not recognized as any valid contact");
            callerId = UTF8(phoneNumber.getFormatted());
        }

        return callerId;
    }
} // namespace app::call
