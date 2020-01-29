#include "CellularCall.hpp"
#include <Modem/ATParser.hpp>
#include <Utils.hpp>
#include <log/log.hpp>
#include <vector>

//#include <Service.hpp>
#include "bsp/rtc/rtc.hpp"

namespace ModemCall
{
    ModemCall::ModemCall(const std::string str)
    {
        const std::string prefix = "+CLCC: ";
        std::string callEntry = str;

        // Check for a valid prefix
        if (callEntry.rfind(prefix, 0) != 0)
        {
            LOG_ERROR("no valid prefix");
            LOG_ERROR("callEntry %s", callEntry.c_str());

            throw std::runtime_error("No valid prefix");
        }
        else
        {
            // remove the prefix
            callEntry.erase(0, prefix.length());
        }

        std::vector<std::string> tokens = utils::split(callEntry, ",");

        auto numberOfTokens = tokens.size();
        if (numberOfTokens != 7 && numberOfTokens != 8)
        {
            LOG_ERROR("wrong number of tokens %u", numberOfTokens);
            throw std::runtime_error("No valid prefix");
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

    std::string ModemCall::to_string(const ModemCall &call)
    {
        std::string str = " <idx> " + std::to_string(call.idx) + " <dir> " + std::to_string(static_cast<uint8_t>(call.dir)) + " <stat> " +
                          std::to_string(static_cast<uint8_t>(call.state)) + " <mode> " + std::to_string(static_cast<uint8_t>(call.mode)) + " <mpty> " +
                          std::to_string(static_cast<uint8_t>(call.isConferenceCall)) + " <number> " + call.phoneNumber + " <type> " +
                          std::to_string(static_cast<uint8_t>(call.type));
        if (!call.phoneBookName.empty())
        {
            str += " <alpha> " + call.phoneBookName;
        }

        return str;
    }
} // namespace ModemCall

namespace CellularCall
{
    bool CellularCall::startCall(const UTF8 &number, const CallType type)
    {
        if (isValid())
        {
            LOG_ERROR("call already set");
            return false;
        }

        time_t timestamp;
        RtcBspError_e rtcErr = bsp::rtc_GetCurrentTimestamp(&timestamp);
        if (rtcErr != RtcBspError_e::RtcBspOK)
        {
            LOG_ERROR("rtc_GetCurrentTimestamp failed with %d error", rtcErr);
            return false;
        }
        clear();
        call.number = number;
        call.type = type;
        call.date = timestamp;
        call.name = number; // temporary set name as number
        uint32_t callId = startCallAction ? startCallAction(call) : 0;
        if (callId == 0)
        {
            LOG_ERROR("startCallAction failed");
            clear();
            return false;
        }
        call.id = callId;

        return true;
    }

    bool CellularCall::setActive()
    {
        if (isValid())
        {
            // startTimer(timerId);
            isActiveCall = true;
            return true;
        }
        return false;
    }

    bool CellularCall::endCall()
    {
        // stopTimer(timerId);

        if (!isValid())
        {
            LOG_ERROR("Trying to update invalid call");
            return false;
        }

        if (isActiveCall)
        {
            call.duration = duration;
        }
        else
        {
            auto callType = call.type;
            switch (callType)
            {
            case CallType::CT_INCOMING: {
                setType(CallType::CT_REJECTED);
            }
            break;

            case CallType::CT_OUTGOING: {
                setType(CallType::CT_MISSED);
            }
            break;

            default:
                LOG_ERROR("Not a valid call type %u", callType);
                return false;
            }
        }

        if (!(endCallAction && endCallAction(call)))
        {
            LOG_ERROR("CalllogUpdate failed, id %u", call.id);
            return false;
        }

        // Calllog entry was updated, ongoingCall can be cleared
        clear();

        return true;
    }
} // namespace CellularCall