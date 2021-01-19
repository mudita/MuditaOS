// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-cellular/CellularCall.hpp"

#include <CalllogRecord.hpp>
#include <PhoneNumber.hpp>
#include <Utils.hpp>
#include <log/log.hpp>
#include <time/time_conversion.hpp>

#include <cinttypes>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace ModemCall
{
    ModemCall::ModemCall(const std::string str)
    {
        const std::string prefix = "+CLCC: ";
        std::string callEntry    = str;

        // Check for a valid prefix
        if (callEntry.rfind(prefix, 0) != 0) {

            throw std::runtime_error("No valid prefix");
        }
        else {
            // remove the prefix
            callEntry.erase(0, prefix.length());
        }

        std::vector<std::string> tokens = utils::split(callEntry, ",");

        auto numberOfTokens = tokens.size();
        if (numberOfTokens != 7 && numberOfTokens != 8) {
            throw std::runtime_error("Wrong number of tokens" + std::to_string(numberOfTokens));
        }

        idx           = std::stoul(tokens[0]);
        auto conv_val = std::stoul(tokens[1]);
        auto tmp_dir  = magic_enum::enum_cast<CallDir>(conv_val);
        if (tmp_dir.has_value()) {
            dir = tmp_dir.value();
        }
        else {
            throw std::runtime_error("dir value out of range CallDir enum - " + tokens[1]);
        }

        conv_val       = std::stoul(tokens[2]);
        auto tmp_state = magic_enum::enum_cast<CallState>(conv_val);
        if (tmp_state.has_value()) {
            state = tmp_state.value();
        }
        else {
            throw std::runtime_error("state value out of range CallState enum - " + tokens[2]);
        }

        conv_val      = std::stoul(tokens[3]);
        auto tmp_mode = magic_enum::enum_cast<CallMode>(conv_val);
        if (tmp_mode.has_value()) {
            mode = tmp_mode.value();
        }
        else {
            throw std::runtime_error("mode value out of range CallMode enum - " + tokens[3]);
        }

        isConferenceCall = static_cast<bool>(std::stoul(tokens[4]));
        phoneNumber      = tokens[5];

        try {
            conv_val = std::stoul(tokens[6]);
        }
        catch (const std::logic_error &) {
            conv_val = 0;
        }
        type = conv_val;

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
    bool CellularCall::startCall(const utils::PhoneNumber::View &number, const CallType type)
    {
        if (isValid()) {
            LOG_ERROR("call already set");
            return false;
        }

        clear();
        CalllogRecord callRec;
        callRec.type        = type;
        callRec.date        = utils::time::Timestamp().getTime();
        callRec.name        = number.getFormatted(); // temporary set name to entered number
        callRec.phoneNumber = number;
        call                = startCallAction ? startCallAction(callRec) : CalllogRecord();
        if (!call.isValid()) {
            LOG_ERROR("startCallAction failed");
            clear();
            return false;
        }

        return true;
    }

    bool CellularCall::setActive()
    {
        if (isValid()) {
            startActiveTime = utils::time::Timestamp();
            isActiveCall    = true;
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

        if (isActiveCall) {
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
                    markUnread();
                }
            } break;

            case CallType::CT_OUTGOING: {
                // do nothing
            } break;

            default:
                LOG_ERROR("Not a valid call type %u", static_cast<int>(callType));
                return false;
            }
        }

        if (!(endCallAction && endCallAction(call))) {
            LOG_ERROR("CalllogUpdate failed, id %" PRIu32, call.ID);
            return false;
        }

        // Calllog entry was updated, ongoingCall can be cleared
        clear();

        return true;
    }
} // namespace CellularCall
