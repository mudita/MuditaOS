#pragma once

#include <PhoneNumber.hpp>
#include <Interface/CalllogRecord.hpp>
#include <time/time_conversion.hpp>

#include <functional>
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

    // TODO: alek: check specification
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
        CallType type;
        std::string phoneBookName; // TODO: alek: This field is defined in the AT+CLCC command resposne but our modem is
                                   // not returning it. Need to verify in modem specification

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
        utils::time::Timestamp startActiveTime = 0;

        void setType(const CallType type)
        {
            call.type = type;
        }

        void clear()
        {
            call.ID           = 0; // 0 - Invalid
            call.presentation = PresentationType::PR_ALLOWED;
            call.date         = 0;
            call.duration     = 0;
            call.type         = CallType::CT_NONE;
            call.name         = "";
            call.contactId    = "";
            isActiveCall      = false;
            startActiveTime   = 0;
            call.phoneNumber.clear();
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
            this->call.contactId   = "1";
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