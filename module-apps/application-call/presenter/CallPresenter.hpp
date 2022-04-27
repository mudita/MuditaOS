// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-apps/application-call/model/CallModel.hpp>
#include <module-apps/application-call/widgets/StateIcons.hpp>
#include "BasePresenter.hpp"

namespace app::call
{
    class CallWindowContract
    {
      public:
        class View
        {
          public:
            virtual void updateDuration(const UTF8 &text, bool isVisible = true) = 0;
            virtual void refreshWindow()                                         = 0;
            virtual void setNavBarForActiveCall()                                = 0;
            virtual void setNavBarForIncomingCall()                              = 0;
            virtual void clearNavBar()                                           = 0;
            virtual void setIncomingCallLayout(bool isValidCallerId)             = 0;
            virtual void setActiveCallLayout()                                   = 0;
            virtual void setCallEndedLayout(bool delayedClose = true)            = 0;
            virtual void updateNumber(const UTF8 &text)                          = 0;

            virtual ~View() noexcept = default;
        };

        class Presenter : public BasePresenter<CallWindowContract::View>
        {
          public:
            Presenter(std::shared_ptr<app::call::AbstractCallModel> callModel);
            ~Presenter() noexcept override;

            void handleCallDurationChange();
            void handleCallStateChange();
            void buildLayout();

            bool handleLeftButton();
            bool handleRightButton();
            bool handleHeadsetOk();
            bool handleDigitButton(const uint32_t &digit);
            void muteCall();
            void unmuteCall();
            void turnLoudspeakerOn();
            void turnLoudspeakerOff();

            void hangUpCall();
            void sendSms(const UTF8 &smsBody);
            utils::PhoneNumber getPhoneNumber();

          private:
            std::shared_ptr<app::call::AbstractCallModel> model;
            UTF8 getCallerId();
            bool isIncomingCall();
            bool isCallInProgress();
        };
    };
} // namespace app::call
