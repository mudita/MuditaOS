// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Interface/CalllogRecord.hpp>
#include <PhoneNumber.hpp>
#include <Tables/CalllogTable.hpp>
#include <time/time_conversion.hpp>
#include <utf8/UTF8.hpp>

#include <cstdint>
#include <functional>
#include <iosfwd>
#include <string>
#include <sys/types.h>
#include <chrono>

namespace ModemCall
{
    enum class CallState : uint8_t
    {
        Active = 0, // 0 active: call in progress (setup was successful)
        Held,       // 1 held: call on hold
        Dialing,    // 2 dialing (MO call): number dialed
        Alerting,   // 3 alerting (MO call): number dialed and the called party is alerted
        Incoming,   // 4 incoming (MT call): incoming call, ringtone played (AT RING notification)
        Waiting // 5 waiting (MT call): call waiting notification while another call is active (if call waiting feature
                // enabled)
    };

    enum class CallDir : uint8_t
    {
        MO = 0, // Mobile originated (MO) call
        MT = 1, // Mobile terminated (MT) call
    };

    enum class CallMode : uint8_t
    {
        Voice = 0,
        Data  = 1,
        FAX   = 2,
    };

    /// Usually contains one of defined values
    /// More details in 3GPP TS 24.008 subclause 10.5.4.7
    enum class CallType : uint8_t
    {
        UknownType      = 129,
        InternationType = 145, // contains the "+" character
        NationalType    = 161,
    };

    struct ModemCall
    {
        int8_t idx;
        CallDir dir;
        CallState state;
        CallMode mode;
        bool isConferenceCall;
        std::string phoneNumber;
        uint8_t type;              /// Usually contains on of values defined in CallType
        std::string phoneBookName; /// This field is defined in the AT+CLCC command response but our modem is
                                   /// not returning it.

        ModemCall()  = delete;
        ~ModemCall() = default;
        ModemCall(const std::string str);

        friend std::ostream &operator<<(std::ostream &out, const ModemCall &call);
    };
} // namespace ModemCall

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
            startActiveTime   = utils::time::TIME_POINT_INVALID;
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

        ~CellularCall() = default;

        void setStartCallAction(const std::function<CalllogRecord(const CalllogRecord &rec)> callAction)
        {
            startCallAction = callAction;
        }

        void setEndCallAction(const std::function<bool(const CalllogRecord &rec)> callAction)
        {
            endCallAction = callAction;
        }

        bool startCall(const utils::PhoneNumber::View &number, const CallType type);

        bool setActive();

        bool endCall(Forced forced = Forced::False);

        bool isValid() const
        {
            return call.ID != 0;
        }
    };
} // namespace CellularCall
