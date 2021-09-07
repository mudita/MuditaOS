// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmMessageHandler.hpp"
#include "ServiceTime.hpp"

#include <module-db/Interface/AlarmEventRecord.hpp>
#include <module-db/Interface/EventRecord.hpp>

#include <service-time/AlarmMessage.hpp>

namespace alarms
{

    AlarmMessageHandler::AlarmMessageHandler(stm::ServiceTime *service,
                                             std::unique_ptr<IAlarmOperations> &&alarmOperations)
        : service{service}, alarmOperations{std::move(alarmOperations)} {};

    auto AlarmMessageHandler::handleGetAlarm(AlarmGetRequestMessage *request)
        -> std::shared_ptr<AlarmGetResponseMessage>
    {
        return handleWithCallback<AlarmGetRequestMessage, AlarmGetResponseMessage, AlarmEventRecord>(
            request, [&](AlarmGetRequestMessage *request, IAlarmOperations::OnGetAlarmProcessed callback) {
                alarmOperations->getAlarm(request->id, callback);
            });
    }

    auto AlarmMessageHandler::handleAddAlarm(AlarmAddRequestMessage *request)
        -> std::shared_ptr<AlarmAddResponseMessage>
    {
        return handleWithCallback<AlarmAddRequestMessage, AlarmAddResponseMessage, bool>(
            request, [&](AlarmAddRequestMessage *request, IAlarmOperations::OnAddAlarmProcessed callback) {
                alarmOperations->addAlarm(request->alarmEvent, callback);
            });
    }

    auto AlarmMessageHandler::handleUpdateAlarm(AlarmUpdateRequestMessage *request)
        -> std::shared_ptr<AlarmUpdateResponseMessage>
    {
        return handleWithCallback<AlarmUpdateRequestMessage, AlarmUpdateResponseMessage, bool>(
            request, [&](AlarmUpdateRequestMessage *request, IAlarmOperations::OnUpdateAlarmProcessed callback) {
                alarmOperations->updateAlarm(request->alarmEvent, callback);
            });
    }

    auto AlarmMessageHandler::handleRemoveAlarm(AlarmRemoveRequestMessage *request)
        -> std::shared_ptr<AlarmRemoveResponseMessage>
    {
        return handleWithCallback<AlarmRemoveRequestMessage, AlarmRemoveResponseMessage, bool>(
            request, [&](AlarmRemoveRequestMessage *request, IAlarmOperations::OnRemoveAlarmProcessed callback) {
                alarmOperations->removeAlarm(request->id, callback);
            });
    }

    auto AlarmMessageHandler::handleGetAlarmsInRange(AlarmsGetInRangeRequestMessage *request)
        -> std::shared_ptr<AlarmsGetInRangeResponseMessage>
    {
        return handleWithCallback<AlarmsGetInRangeRequestMessage,
                                  AlarmsGetInRangeResponseMessage,
                                  std::pair<std::vector<AlarmEventRecord>, std::uint32_t>>(
            request,
            [&](AlarmsGetInRangeRequestMessage *request, IAlarmOperations::OnGetAlarmsInRangeProcessed callback) {
                alarmOperations->getAlarmsInRange(
                    request->start, request->end, request->offset, request->limit, std::move(callback));
            });
    }

    auto AlarmMessageHandler::handleGetNextSingleEvents(AlarmGetNextSingleEventsRequestMessage *request)
        -> std::shared_ptr<AlarmGetNextSingleEventsResponseMessage>
    {
        return handleWithCallback<AlarmGetNextSingleEventsRequestMessage,
                                  AlarmGetNextSingleEventsResponseMessage,
                                  std::vector<SingleEventRecord>>(
            request,
            [&](AlarmGetNextSingleEventsRequestMessage *request, IAlarmOperations::OnGetNextSingleProcessed callback) {
                alarmOperations->getNextSingleEvents(TimePointNow(), callback);
            });
    }

    auto AlarmMessageHandler::handleMinuteUpdated() -> void
    {
        alarmOperations->minuteUpdated(TimePointNow());
    }

    auto AlarmMessageHandler::addAlarmExecutionHandler(const alarms::AlarmType type,
                                                       const std::shared_ptr<alarms::AlarmHandler> handler) -> void
    {
        alarmOperations->addAlarmExecutionHandler(type, handler);
    }

    template <class RequestType, class ResponseType, class CallbackParamType>
    auto AlarmMessageHandler::handleWithCallback(
        RequestType *request,
        const std::function<void(RequestType *request, std::function<void(CallbackParamType)>)>
            &alarmOperationsCallback) -> std::shared_ptr<ResponseType>
    {
        auto callback = [&, requestPtr = std::make_shared<RequestType>(*request)](CallbackParamType param) {
            auto response   = std::make_shared<ResponseType>(param);
            service->bus.sendResponse(response, requestPtr);
        };

        alarmOperationsCallback(request, callback);
        return sys::MessageNone{};
    }

} // namespace alarms
