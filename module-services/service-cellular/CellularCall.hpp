
/*
 * @file CellularCall.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 11.07.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#pragma once

#include "Interface/CalllogRecord.hpp"
#include <string>

namespace ModemCall
{
    enum class CallState : uint8_t
    {
        Active = 0, // 0 active: call in progress (setup was successful)
        Held,       // 1 held: call on hold
        Dialing,    // 2 dialing (MO call): number dialed
        Alerting,   // 3 alerting (MO call): number dialed and the called party is alerted
        Incoming,   // 4 incoming (MT call): incoming call, ringtone played (AT RING notification)
        Waiting     // 5 waiting (MT call): call waiting notification while another call is active (if call waiting feature enabled)
    };

    enum class CallDir : uint8_t
    {
        MO = 0, // Mobile originated (MO) call
        MT = 1, // Mobile terminated (MT) call
    };

    enum class CallMode : uint8_t
    {
        Voice = 0,
        Data = 1,
        FAX = 2,
    };

    // TODO: alek: check specification
    enum class CallType : uint8_t
    {
        UknownType = 129,
        InternationType = 145, // contains the "+" character
        NationalType = 161,
    };

    struct ModemCall
    {
        int8_t idx; // TODO: alek: need to check what that is
        CallDir dir;
        CallState state;
        CallMode mode;
        bool isConferenceCall;
        std::string phoneNumber;
        CallType type;
        std::string phoneBookName; // TODO: alek: seems like it is not existising in the recieved data
                                   // but it is defined in specification
        // CallState previousState;

        ModemCall() = delete;
        ~ModemCall() = default;
        ModemCall(const std::string str);

        std::string getStringRepresntation();
    };
} // namespace ModemCall

namespace CellularCall
{
    class CellularCall
    {
        CalllogRecord call;

      public:
        CellularCall() = default;
        ~CellularCall() = default;
        CellularCall(const UTF8 &number, CallType type = CallType::CT_NONE, time_t date = 0, time_t duration = 0)
        {
            this->call.id = 0; // 0 - Invalid
            this->call.number = number;
            this->call.presentation = PresentationType::PR_ALLOWED;
            this->call.date = date;
            this->call.duration = duration;
            this->call.type = type;
            this->call.name = "No Name";
            this->call.contactId = "1";
        }

        bool isValid()
        {
            return call.id != 0;
        }

        const CalllogRecord &getCallRecord() const
        {
            return call;
        };
    };
} // namespace CellularCall