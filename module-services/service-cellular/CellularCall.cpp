#include "CellularCall.hpp"
#include <Modem/ATParser.hpp>
#include <Utils.hpp>
#include <log/log.hpp>
#include <vector>

#include "bsp/rtc/rtc.hpp"
#include <sstream>

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

    std::ostream &operator<<(std::ostream &out, const ModemCall &call)
    {
        out << " <idx> " << call.idx << " <dir> " << static_cast<uint32_t>(call.dir) << " <stat> " << static_cast<uint32_t>(call.state) << " <mode> "
            << static_cast<uint32_t>(call.mode) << " <mpty> " << static_cast<uint32_t>(call.isConferenceCall) << " <number> " << call.phoneNumber << " <type> "
            << static_cast<uint32_t>(call.type);

        if (!call.phoneBookName.empty())
        {
            out << " <alpha> " << call.phoneBookName;
        }

        return out;
    }
} // namespace ModemCall

namespace
{
    // TODO: alek: possibly make a new general API for this
    time_t getCurrentTimeStamp()
    {
        time_t timestamp;
        RtcBspError_e rtcErr = bsp::rtc_GetCurrentTimestamp(&timestamp);
        if (rtcErr != RtcBspError_e::RtcBspOK)
        {
            LOG_ERROR("rtc_GetCurrentTimestamp failed with %d error", rtcErr);
            timestamp = 0;
        }

        return timestamp;
    }
} // namespace

namespace CellularCall
{
    bool CellularCall::startCall(const UTF8 &number, const CallType type)
    {
        if (isValid())
        {
            LOG_ERROR("call already set");
            return false;
        }

        clear();
        call.number = number;
        call.type = type;
        call.date = getCurrentTimeStamp();
        call.name = number; // temporary set name as number
        uint32_t callId = startCallAction ? startCallAction(call) : 0;
        if (callId == 0)
        {
            LOG_ERROR("startCallAction failed");
            clear();
            return false;
        }
        call.ID = callId;

        return true;
    }

    bool CellularCall::setActive()
    {
        if (isValid())
        {
            startActiveTime = getCurrentTimeStamp();
            isActiveCall = true;
            return true;
        }
        return false;
    }

    bool CellularCall::endCall()
    {

        if (!isValid())
        {
            LOG_ERROR("Trying to update invalid call");
            return false;
        }

        if (isActiveCall)
        {
            time_t endTime = getCurrentTimeStamp();
            call.duration = endTime > startActiveTime ? endTime - startActiveTime : 0;
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
            LOG_ERROR("CalllogUpdate failed, id %u", call.ID);
            return false;
        }

        // Calllog entry was updated, ongoingCall can be cleared
        clear();

        return true;
    }
} // namespace CellularCall