
/*
 * @file Common.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 11.07.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#pragma once
#include <log/log.hpp>
#include <string>

namespace CellularCall
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

    struct CellularCall
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

        CellularCall() = delete;
        ~CellularCall() = default;
        CellularCall(const std::string str)
        {
            const std::string prefix = "+CLCC: ";
            std::string callEntry = str;
            // Check for a valid prefix
            if (callEntry.rfind(prefix, 0) != 0)
            {
                LOG_ERROR("no valid prefix");
                LOG_ERROR("callEntry %s", callEntry.c_str());

                throw 20; // TODO: alek: add proper exceptions
            }
            else
            {
                // remove the prefix
                callEntry.erase(0, prefix.length());
            }

            std::vector<std::string> tokens = ATParser::Tokenizer(callEntry, ",");

            auto numberOfTokens = tokens.size();
            if (numberOfTokens != 7 && numberOfTokens != 8)
            {
                LOG_ERROR("wrong number of tokens %u", numberOfTokens);
                throw 20;
            }
            // TODO: alek: add paramters validation
            idx = std::stoul(tokens[0]);
            dir = static_cast<CallDir>(std::stoul(tokens[1]));
            state = static_cast<CallState>(std::stoul(tokens[2]));
            mode = static_cast<CallMode>(std::stoul(tokens[3]));
            isConferenceCall = static_cast<bool>(std::stoul(tokens[4]));
            phoneNumber = tokens[5];
            type = static_cast<CallType>(std::stoul(tokens[6]));
            if (numberOfTokens == 8)
            {
                phoneBookName = tokens[7];
            }
        }

        std::string getStringRepresntation()
        {
            std::string str = " <idx> " + std::to_string(idx) + " <dir> " + std::to_string(static_cast<uint8_t>(dir)) + " <stat> " +
                              std::to_string(static_cast<uint8_t>(state)) + " <mode> " + std::to_string(static_cast<uint8_t>(mode)) + " <mpty> " +
                              std::to_string(static_cast<uint8_t>(isConferenceCall)) + " <number> " + phoneNumber + " <type> " +
                              std::to_string(static_cast<uint8_t>(type));
            if (!phoneBookName.empty())
            {
                str += " <alpha> " + phoneBookName;
            }

            return str;
        }
    };
} // namespace CellularCall
