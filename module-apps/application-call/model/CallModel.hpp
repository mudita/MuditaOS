// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-utils/time/time/time_conversion.hpp>
#include <module-utils/phonenumber/PhoneNumber.hpp>

#include <products/PurePhone/apps/include/Application.hpp>

#include <cstdint>

namespace app::call
{
    enum class CallState
    {
        None,
        Incoming,
        Outgoing,
        Active,
        Ended,
        Rejected,
        Disconnecting
    };

    inline auto c_str(app::call::CallState state) -> const char *
    {
        switch (state) {
        case CallState::None:
            return "None";
        case CallState::Incoming:
            return "Incoming";
        case CallState::Outgoing:
            return "Outgoing";
        case CallState::Active:
            return "Active";
        case CallState::Ended:
            return "Ended";
        case CallState::Rejected:
            return "Ended";
        case CallState::Disconnecting:
            return "Disconnecting";
        }
        return "";
    }

    class AbstractCallModel
    {
      public:
        virtual ~AbstractCallModel() noexcept                        = default;
        virtual time_t getTime()                                     = 0;
        virtual void setTime(time_t newTime)                         = 0;
        virtual CallState getState()                                 = 0;
        virtual void setState(CallState newState)                    = 0;
        virtual void setPhoneNumber(const utils::PhoneNumber number) = 0;
        virtual utils::PhoneNumber getPhoneNumber()                  = 0;
        virtual UTF8 getCallerId()                                   = 0;

        virtual void hangUpCall()                               = 0;
        virtual void answerCall()                               = 0;
        virtual bool sendSms(const UTF8 &smsBody)               = 0;
        virtual void transmitDtmfTone(const uint8_t &digitCode) = 0;
        virtual void muteCall()                                 = 0;
        virtual void unmuteCall()                               = 0;
        virtual void turnLoudspeakerOn()                        = 0;
        virtual void turnLoudspeakerOff()                       = 0;

        virtual void turnOnKeypadBacklight()  = 0;
        virtual void turnOffKeypadBacklight() = 0;

        virtual void clear()                                                       = 0;
        virtual void attachDurationChangeCallback(std::function<void()> callback)  = 0;
        virtual void attachCallStateChangeCallback(std::function<void()> callback) = 0;
    };

    class CallModel : public AbstractCallModel
    {
      public:
        CallModel() = delete;
        explicit CallModel(app::Application *app) : application(app){};
        time_t getTime() final;
        void setTime(time_t newTime) final;
        CallState getState() final;
        void setState(CallState newState) final;
        void setPhoneNumber(const utils::PhoneNumber number) final;
        utils::PhoneNumber getPhoneNumber() final;
        UTF8 getCallerId() final;

        void hangUpCall() final;
        void answerCall() final;
        bool sendSms(const UTF8 &smsBody) final;
        void transmitDtmfTone(const uint8_t &digitCode) final;
        void muteCall();
        void unmuteCall();
        void turnLoudspeakerOn();
        void turnLoudspeakerOff();

        void turnOnKeypadBacklight() final;
        void turnOffKeypadBacklight() final;

        void clear() final;
        void attachDurationChangeCallback(std::function<void()> callback) final;
        void attachCallStateChangeCallback(std::function<void()> callback) final;

      private:
        Application *application;
        CallState callState = CallState::None;
        time_t callTime     = 0;

        std::function<void()> notifyPresenterOnDurationChange;
        std::function<void()> notifyPresenterOnCallStateChange;

        utils::PhoneNumber phoneNumber;
        UTF8 callerId;

        bool callWasRejected = false;
    };
} // namespace app::call
