// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-cellular/call/CellularCall.hpp"
#include "service-cellular/call/CallRingGuard.hpp"
#include "service-cellular/ServiceCellular.hpp"
#include "service-db/agents/settings/SystemSettings.hpp"

#include <queries/notifications/QueryNotificationsIncrement.hpp>
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
    Call::Call(ServiceCellular &owner) : owner(owner), audio(owner), gui(owner), db(owner)
    {
        utils::PhoneNumber::View number = utils::PhoneNumber::View();
        const CallType type             = CallType::CT_NONE;
        const time_t date               = 0;
        const time_t duration           = 0;

        clear();

        this->call.phoneNumber = number;
        this->call.date        = date;
        this->call.duration    = duration;
        this->call.type        = type;
        this->call.name        = number.getEntered(); // temporary set number as name
    }

    bool Call::handleRING()
    {
        if (callRingGuard(*this)) {
            startCall(utils::PhoneNumber::View(), CallType::CT_INCOMING);
            audio.play();

            if (!wasRinging) {
                wasRinging = true;
                gui.notifyRING();
            }
            return true;
        }
        return false;
    }

    bool Call::handleCLIP(const utils::PhoneNumber::View &number)
    {
        setNumber(number);
        if (callClipGuard(*this)) {
            startCall(number, CallType::CT_INCOMING);
            audio.play();
        }

        if (callClipGuard(*this) || callRingGuard(*this)) {
            if (!isNumberDisplayed) {
                isNumberDisplayed = true;
                gui.notifyCLIP(number);
            }
            return true;
        }

        if (callDNDGuard(*this)) {
            db.incrementNotAnsweredCallsNotification(number);
        }

        return false;
    }

    bool Call::startOutgoing(const utils::PhoneNumber::View &number)
    {
        return startCall(number, CallType::CT_OUTGOING);
    }

    bool Call::startCall(const utils::PhoneNumber::View &number, const CallType type)
    {
        LOG_INFO("starting call");

        if (isValid()) {
            LOG_ERROR("call already established");
            return false;
        }

        if (cpuSentinel) {
            cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_6);
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

    bool Call::setActive()
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

    bool Call::endCall(Forced forced)
    {
        audio.stop();
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

    void Call::setNumber(const utils::PhoneNumber::View &number)
    {
        call.presentation = number.getFormatted().empty() ? PresentationType::PR_UNKNOWN : PresentationType::PR_ALLOWED;
        call.phoneNumber  = number;
    }

    void Call::setCpuSentinel(std::shared_ptr<sys::CpuSentinel> sentinel)
    {
        cpuSentinel = std::move(sentinel);
    }

    bool Call::Operations::areCallsFromFavouritesEnabled()
    {
        return call.owner.areCallsFromFavouritesEnabled();
    }

    bool Call::Operations::isNumberInFavourites()
    {
        return DBServiceAPI::IsContactInFavourites(&call.owner, call.call.phoneNumber);
    }

} // namespace CellularCall
