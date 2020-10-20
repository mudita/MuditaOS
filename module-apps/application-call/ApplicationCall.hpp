// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include "Service/Message.hpp"
#include "Service/Timer.hpp"
#include "SystemManager/SystemManager.hpp"
#include <service-cellular/api/CellularServiceAPI.hpp>
#include <time/time_conversion.hpp>

namespace app
{
    inline const std::string name_call      = "ApplicationCall";
    constexpr std::uint16_t call_stack_size = 8192;

    namespace window
    {
        inline const std::string name_call              = "CallWindow";
        inline const std::string name_enterNumber       = "EnterNumberWindow";
        inline const std::string name_emergencyCall     = "EmergencyCallWindow";
        inline const std::string name_duplicatedContact = "DuplicatedContactWindow";
        inline const std::string name_dialogConfirm     = "DialogConfirm";
    } // namespace window

    inline const std::string ringtone_path = "assets/audio/ringtone.wav"; // Should bo moved to database

    class ApplicationCall : public Application
    {
      private:
        void CallAbortHandler();
        void CallActiveHandler();
        void IncomingCallHandler(const CellularCallMessage *const msg);
        void RingingHandler(const CellularCallMessage *const msg);

      protected:
        sys::ms callDelayedStopTime = 3000;

      public:
        ApplicationCall(std::string name = name_call, std::string parent = "", bool startBackgound = false);
        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;
        void setDisplayedNumber(std::string num);
        const std::string &getDisplayedNumber();

        void handleCallEvent(const std::string &number);
        void handleAddContactEvent(const std::string &number);

        bool showNotification(std::function<bool()> action);
        void transmitDtmfTone(uint32_t digit);
        auto getDelayedStopTime() const
        {
            return callDelayedStopTime;
        }

        void stopAudio();
        void startRinging();
        void startRouting();
    };
} /* namespace app */
