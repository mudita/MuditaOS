#include "CellularCall.hpp"
#include <Modem/ATParser.hpp>
#include <Utils.hpp>
#include <log/log.hpp>
#include <vector>

#include "bsp/rtc/rtc.hpp"
#include <sstream>
#include <time/time_conversion.hpp>

namespace ModemCall
{
    ModemCall::ModemCall(const std::string str)
    {
        const std::string prefix = "+CLCC: ";
        std::string callEntry    = str;

        // Check for a valid prefix
        if (callEntry.rfind(prefix, 0) != 0) {
            LOG_ERROR("no valid prefix");
            LOG_ERROR("callEntry %s", callEntry.c_str());

            throw std::runtime_error("No valid prefix");
        }
        else {
            // remove the prefix
            callEntry.erase(0, prefix.length());
        }

        std::vector<std::string> tokens = utils::split(callEntry, ",");

        auto numberOfTokens = tokens.size();
        if (numberOfTokens != 7 && numberOfTokens != 8) {
            LOG_ERROR("wrong number of tokens %u", numberOfTokens);
            throw std::runtime_error("No valid prefix");
        }
        // TODO: alek: add paramters validation
        idx              = std::stoul(tokens[0]);
        dir              = static_cast<CallDir>(std::stoul(tokens[1]));
        state            = static_cast<CallState>(std::stoul(tokens[2]));
        mode             = static_cast<CallMode>(std::stoul(tokens[3]));
        isConferenceCall = static_cast<bool>(std::stoul(tokens[4]));
        phoneNumber      = tokens[5];
        type             = static_cast<CallType>(std::stoul(tokens[6]));
        if (numberOfTokens == 8) {
            phoneBookName = tokens[7];
        }
    }

    std::ostream &operator<<(std::ostream &out, const ModemCall &call)
    {
        out << " <idx> " << call.idx << " <dir> " << static_cast<uint32_t>(call.dir) << " <stat> "
            << static_cast<uint32_t>(call.state) << " <mode> " << static_cast<uint32_t>(call.mode) << " <mpty> "
            << static_cast<uint32_t>(call.isConferenceCall) << " <number> " << call.phoneNumber << " <type> "
            << static_cast<uint32_t>(call.type);

        if (!call.phoneBookName.empty()) {
            out << " <alpha> " << call.phoneBookName;
        }

        return out;
    }
} // namespace ModemCall

namespace CellularCall
{
    bool CellularCall::startCall(const UTF8 &number, const CallType type)
    {
        if (isValid()) {
            LOG_ERROR("call already set");
            return false;
        }

        clear();
        CalllogRecord callRec;
        callRec.number = number;
        callRec.type = type;
        callRec.date   = utils::time::Timestamp().getTime();
        callRec.name = number; // temporary set name as number
        call = startCallAction ? startCallAction(callRec) : CalllogRecord();
        if (call.ID == DB_ID_NONE)
        {
            LOG_ERROR("startCallAction failed");
            clear();
            return false;
        }

        return true;
    }

    bool CellularCall::setActive()
    {
        if (isValid())
        {
            startActiveTime = utils::time::Timestamp();
            isActiveCall = true;
            return true;
        }
        return false;
    }

    bool CellularCall::endCall(Forced forced)
    {
        if (!isValid()) {
            LOG_ERROR("Trying to update invalid call");
            return false;
        }

        if (isActiveCall)
        {
            auto endTime  = utils::time::Timestamp();
            call.duration = (endTime - startActiveTime).get();
        }
        else {
            auto callType = call.type;
            switch (callType) {
            case CallType::CT_INCOMING: {
                if (forced == Forced::True) {
                    setType(CallType::CT_REJECTED);
                }
                else {
                    setType(CallType::CT_MISSED);
                }
            } break;

            case CallType::CT_OUTGOING: {
                // do nothing
            } break;

            default:
                LOG_ERROR("Not a valid call type %u", callType);
                return false;
            }
        }

        if (!(endCallAction && endCallAction(call))) {
            LOG_ERROR("CalllogUpdate failed, id %u", call.ID);
            return false;
        }

        // Calllog entry was updated, ongoingCall can be cleared
        clear();

        return true;
    }
} // namespace CellularCall
