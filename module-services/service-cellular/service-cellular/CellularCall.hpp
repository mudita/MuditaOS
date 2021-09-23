// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Interface/CalllogRecord.hpp>
#include <PhoneNumber.hpp>
#include <Tables/CalllogTable.hpp>
#include <time/time_conversion.hpp>
#include <utf8/UTF8.hpp>
#include <Utils.hpp>

#include <cstdint>
#include <functional>
#include <iosfwd>
#include <string>
#include <sys/types.h>
#include <SystemManager/CpuSentinel.hpp>

namespace CellularCall
{
    enum class Forced : bool
    {
        False,
        True
    };

    class CellularCall
    {
        CalllogRecord call;
        bool isActiveCall = false;
        std::function<CalllogRecord(const CalllogRecord &rec)> startCallAction;
        std::function<bool(const CalllogRecord &rec)> endCallAction;
        utils::time::Timestamp startActiveTime;

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
            call         = CalllogRecord();
            isActiveCall = false;
            startActiveTime.set_time(0);
        }

      public:
        CellularCall(utils::PhoneNumber::View number = utils::PhoneNumber::View(),
                     const CallType type             = CallType::CT_NONE,
                     const time_t date               = 0,
                     const time_t duration           = 0)
        {
            clear();
            this->call.phoneNumber = number;
            this->call.date        = date;
            this->call.duration    = duration;
            this->call.type        = type;
            this->call.name        = number.getEntered(); // temporary set number as name
            this->call.contactId   = 0;
        }

        void setStartCallAction(const std::function<CalllogRecord(const CalllogRecord &rec)> callAction)
        {
            startCallAction = callAction;
        }

        void setEndCallAction(const std::function<bool(const CalllogRecord &rec)> callAction)
        {
            endCallAction = callAction;
        }

        bool setActive();
        void setNumber(const utils::PhoneNumber::View &number);

        bool startCall(const utils::PhoneNumber::View &number, const CallType type);
        bool endCall(Forced forced = Forced::False);

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
    };
} // namespace CellularCall
