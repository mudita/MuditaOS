// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-apps/application-call/model/CallModel.hpp>
#include <module-apps/application-call/presenter/CallPresenter.hpp>

namespace app::call
{

    class ViewMock : public CallWindowContract::View
    {
      public:
        void updateDuration(const UTF8 &text, bool isVisible = true) override
        {
            duration = text;
        };
        void refreshWindow() override
        {
            windowRefreshed = true;
        };
        void setNavBarForActiveCall() override{};
        void setNavBarForIncomingCall() override{};
        void clearNavBar() override{};
        void setIncomingCallLayout(bool isValidCallerId) override
        {
            layoutShowed = LayoutShowed::Incoming;
        };
        void setActiveCallLayout() override
        {
            layoutShowed = LayoutShowed::Active;
        };
        ;
        void setCallEndedLayout(bool delayedClose = true) override
        {
            layoutShowed = LayoutShowed::Ended;
        };

        void updateNumber(const UTF8 &text) override
        {
            number = text;
        };

        bool windowRefreshed = false;

        enum class LayoutShowed
        {
            Incoming,
            Active,
            Ended,
            None
        };

        LayoutShowed layoutShowed = LayoutShowed::None;
        UTF8 number;
        UTF8 duration;
    };

    class ModelMock : public AbstractCallModel
    {
      public:
        time_t getTime() override
        {
            return callTime;
        };
        void setTime(time_t newTime) override
        {
            callTime = newTime;
        };
        CallState getState() override
        {
            return callState;
        };
        void setState(CallState newState) override
        {
            callState = newState;
        };
        void setPhoneNumber(const utils::PhoneNumber number) override{};
        utils::PhoneNumber getPhoneNumber() override
        {
            return phoneNumber;
        };
        UTF8 getCallerId() override
        {
            return callerId;
        };

        void hangUpCall() override
        {
            hangupCallCalled = true;
        };
        void answerCall() override
        {
            answerCallCalled = true;
        };
        bool sendSms(const UTF8 &smsBody) override
        {
            return true;
        }
        void transmitDtmfTone(const uint32_t &digit) override{};
        void muteCall() override{};
        void unmuteCall() override{};
        void turnLoudspeakerOn() override{};
        void turnLoudspeakerOff() override{};

        void turnOnKeypadBacklight() override{};
        void turnOffKeypadBacklight() override{};

        void clear() override{};
        void attachDurationChangeCallback(std::function<void()> callback) override{};
        void attachCallStateChangeCallback(std::function<void()> callback) override{};

        bool hangupCallCalled = false;
        bool answerCallCalled = false;

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
