// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "call/CallAudio.hpp"
#include "call/CallGUI.hpp"
#include "call/CallDB.hpp"
#include "PhoneModes/PhoneMode.hpp"
#include <service-cellular/CellularMessage.hpp>
#include <Interface/CalllogRecord.hpp>
#include <SystemManager/CpuSentinel.hpp>
#include <PhoneNumber.hpp>
#include <Tables/CalllogTable.hpp>
#include <time/time_conversion.hpp>
#include <utf8/UTF8.hpp>
#include <Utils.hpp>

#include <cstdint>
#include <memory>
#include <functional>
#include <iosfwd>
#include <string>
#include <sys/types.h>

class ServiceCellular;

namespace CellularCall
{
    enum class Forced : bool
    {
        False,
        True
    };

    class Call
    {
        CalllogRecord call;
        bool isActiveCall      = false;
        bool wasRinging        = false;
        bool isNumberDisplayed = false;
        std::function<CalllogRecord(const CalllogRecord &rec)> startCallAction;
        std::function<bool(const CalllogRecord &rec)> endCallAction;
        utils::time::Timestamp startActiveTime;
        time_t callDurationTime = 0;

        std::shared_ptr<sys::CpuSentinel> cpuSentinel;

        void setType(const CallType type)
        {
            call.type = type;
        }

        void markUnread()
        {
            call.isRead = false;
        }

        void clear()
        {
            call              = CalllogRecord();
            isActiveCall      = false;
            wasRinging        = false;
            isNumberDisplayed = false;
            startActiveTime.set_time(0);
            callDurationTime = 0;
        }

        bool startCall(const utils::PhoneNumber::View &number, const CallType type);

        ServiceCellular &owner;
        CallRingAudio audio;
        CallGUI gui;
        CallDB db;

      public:
        void setMode(sys::phone_modes::PhoneMode mode)
        {
            this->mode = mode;
        }

        mutable sys::phone_modes::PhoneMode mode;
        explicit Call(ServiceCellular &owner);

        void setStartCallAction(const std::function<CalllogRecord(const CalllogRecord &rec)> callAction)
        {
            startCallAction = callAction;
        }

        void ongoingCallShowUI();
        void setEndCallAction(const std::function<bool(const CalllogRecord &rec)> callAction)
        {
            endCallAction = callAction;
        }

        bool setActive();
        void setNumber(const utils::PhoneNumber::View &number);
        bool ringAudioOnClip();

        bool startOutgoing(const utils::PhoneNumber::View &number);
        bool handleRING();
        bool handleCLIP(const utils::PhoneNumber::View &number);
        bool endCall(Forced forced = Forced::False);

        void handleCallAudioEventRequest(cellular::CallAudioEventRequest::EventType event);

        bool isValid() const
        {
            return call.ID != DB_ID_NONE;
        }

        bool isActive() const
        {
            return isActiveCall;
        }

        [[nodiscard]] CallType getType() const noexcept
        {
            return call.type;
        }

        void setCpuSentinel(std::shared_ptr<sys::CpuSentinel> sentinel);

        void handleCallDurationTimer();

        struct Operations
        {
            bool areCallsFromFavouritesEnabled();
            bool isNumberInFavourites();

          private:
            friend Call;
            Call &call;
            explicit Operations(Call &owner) : call(owner)
            {}
        };
        Operations operations = Operations(*this);
        friend Operations;
    };
} // namespace CellularCall
