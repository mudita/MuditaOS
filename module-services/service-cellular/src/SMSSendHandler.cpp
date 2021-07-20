// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SMSSendHandler.hpp"
#include <queries/messages/sms/QuerySMSSearchByType.hpp>
#include <queries/messages/sms/QuerySMSUpdate.hpp>

#if DEBUG_SERVICE_CELLULAR > 0
#define log_debug(format, ...) LOG_DEBUG(format, ##__VA_ARGS__)
#else
#define log_debug(...) (void)(0)
#endif

namespace cellular::internal::sms
{
    SMSSendHandler::SMSSendHandler() : currentState(std::make_unique<IdleState>(*this))
    {}

    void SMSSendHandler::handleStateChange(OptionalState state)
    {
        if (state) {
            log_debug("Switched from %s to %s", currentState->toStr().c_str(), state.value()->toStr().c_str());
            currentState = std::move(state.value());
        }
    }

    void SMSSendHandler::handleDBNotification()
    {
        handleStateChange(currentState->handle({}));
    }
    void SMSSendHandler::handleIncomingDbRecord(SMSRecord &record)
    {
        handleStateChange(currentState->handle(record));
    }

    void SMSSendHandler::handleNoMoreDbRecords()
    {
        handleStateChange(currentState->handle(SMSRecord{}));
    }

    bool State::isNotification(const std::optional<const SMSRecord> record)
    {
        return !record;
    }
    bool State::isRecordEmpty(const std::optional<const SMSRecord> record)
    {
        return record && !record->isValid();
    }

    OptionalState IdleState::handle(std::optional<SMSRecord> record)
    {
        if (!isNotification(record) || isRecordEmpty(record)) {
            return {};
        }

        context.onSendQuery(db::Interface::Name::SMS,
                            std::make_unique<db::query::SMSSearchByType>(SMSType::QUEUED, 0, 1));

        return std::make_unique<ProcessQueryState>(context);
    }

    OptionalState ProcessQueryState::handle(std::optional<SMSRecord> record)
    {
        if (isNotification(record)) {
            return {};
        }

        if (isRecordEmpty(record)) {
            return std::make_unique<IdleState>(context);
        }

        if (context.onGetOfflineMode()) {
            record->type = SMSType::FAILED;
            context.onSendQuery(db::Interface::Name::SMS, std::make_unique<db::query::SMSUpdate>(std::move(*record)));
        }
        else {
            context.onSend(*record);
        }

        // Check if there are more queued records
        context.onSendQuery(db::Interface::Name::SMS,
                            std::make_unique<db::query::SMSSearchByType>(SMSType::QUEUED, 0, 1));
        return {};
    }

    std::string ProcessQueryState::toStr() const
    {
        return "ProcessQuery";
    }

    std::string IdleState::toStr() const
    {
        return "Idle";
    }
} // namespace cellular::internal::sms
