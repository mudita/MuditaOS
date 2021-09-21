// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-cellular/CellularCall.hpp"

#include <CalllogRecord.hpp>
#include <PhoneNumber.hpp>
#include <Utils.hpp>
#include <log/log.hpp>

#include <cinttypes>
#include <ctime>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace CellularCall
{
    bool CellularCall::startCall(const utils::PhoneNumber::View &number, const CallType type)
    {
        LOG_INFO("starting call");

        if (isValid()) {
            LOG_ERROR("call already established");
            return false;
        }

        if (cpuSentinel) {
            cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyHz::Level_6);
        }

        clear();
        CalllogRecord callRec{type, number};
        call = startCallAction ? startCallAction(callRec) : CalllogRecord();
        if (!call.isValid()) {
            LOG_ERROR("failed to obtain a call log record");
            clear();
            if (cpuSentinel) {
                cpuSentinel->ReleaseMinimumFrequency();
            }
            LOG_INFO("failed to start call");
            return false;
        }

        LOG_INFO("call started");
        return true;
    }

    bool CellularCall::setActive()
    {
        if (isValid()) {
            startActiveTime = utils::time::getCurrentTimestamp();
            isActiveCall    = true;
            LOG_INFO("set call active");
            return true;
        }
        LOG_ERROR("no valid call to activate");
        return false;
    }

    bool CellularCall::endCall(Forced forced)
    {
        LOG_INFO("ending call");
        if (!isValid()) {
            LOG_ERROR("no call to end");
            return false;
        }

        if (cpuSentinel) {
            cpuSentinel->ReleaseMinimumFrequency();
        }

        if (isActiveCall) {
            auto endTime  = utils::time::getCurrentTimestamp();
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

        LOG_INFO("call ended");
        return true;
    }

    void CellularCall::setNumber(const utils::PhoneNumber::View &number)
    {
        call.presentation = number.getFormatted().empty() ? PresentationType::PR_UNKNOWN : PresentationType::PR_ALLOWED;
        call.phoneNumber  = number;
    }

    void CellularCall::setCpuSentinel(std::shared_ptr<sys::CpuSentinel> sentinel)
    {
        cpuSentinel = std::move(sentinel);
    }

} // namespace CellularCall
