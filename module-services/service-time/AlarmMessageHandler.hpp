// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmOperations.hpp"

#include <service-time/AlarmMessage.hpp>

#include <Service/Service.hpp>

namespace stm
{
    class ServiceTime;
}

namespace alarms
{
    class AlarmMessageHandler
    {
      public:
        AlarmMessageHandler(stm::ServiceTime *service, std::unique_ptr<IAlarmOperations> &&alarmOperations);

        auto handleTimeUpdate(TimePoint time) -> void;
        auto handleGetAlarm(AlarmGetRequestMessage *request) -> std::shared_ptr<AlarmGetResponseMessage>;
        auto handleAddAlarm(AlarmAddRequestMessage *request) -> std::shared_ptr<AlarmAddResponseMessage>;
        auto handleUpdateAlarm(AlarmUpdateRequestMessage *request) -> std::shared_ptr<AlarmUpdateResponseMessage>;
        auto handleRemoveAlarm(AlarmRemoveRequestMessage *request) -> std::shared_ptr<AlarmRemoveResponseMessage>;
        auto handleGetAlarmsInRange(AlarmsGetInRangeRequestMessage *request)
            -> std::shared_ptr<AlarmsGetInRangeResponseMessage>;
        auto handleGetNextSingleEvents(AlarmGetNextSingleEventsRequestMessage *request)
            -> std::shared_ptr<AlarmGetNextSingleEventsResponseMessage>;
        auto handleTurnOffRingingAlarm(RingingAlarmTurnOffRequestMessage *request)
            -> std::shared_ptr<RingingAlarmTurnOffResponseMessage>;
        auto handleTurnOffSnooze(TurnOffSnoozeRequestMessage *request) -> std::shared_ptr<TurnOffSnoozeResponseMessage>;
        auto handleSnoozeRingingAlarm(RingingAlarmSnoozeRequestMessage *request)
            -> std::shared_ptr<RingingAlarmSnoozeResponseMessage>;
        auto handlePostponeSnooze(PostponeSnoozeRequestMessage *request)
            -> std::shared_ptr<PostponeSnoozeResponseMessage>;
        auto handleMinuteUpdated() -> void;
        auto handleStopAllSnoozedAlarms() -> void;
        auto handleAddSnoozedAlarmCountChangeCallback(AlarmOperationsCommon::OnSnoozedAlarmsCountChange callback)
            -> void;
        auto handleAddActiveAlarmCountChangeCallback(AlarmOperationsCommon::OnActiveAlarmCountChange callback) -> void;
        auto handleToggleAll(AlarmToggleAllRequestMessage *request) -> std::shared_ptr<AlarmToggleAllResponseMessage>;
        auto handleGetSnoozedAlarms(GetSnoozedAlarmsRequestMessage *request)
            -> std::shared_ptr<GetSnoozedAlarmsResponseMessage>;

      private:
        stm::ServiceTime *service = nullptr;
        std::unique_ptr<IAlarmOperations> alarmOperations;

        template <class RequestType, class ResponseType, class CallbackParamType>
        auto handleWithCallback(RequestType *request,
                                const std::function<void(RequestType *request, std::function<void(CallbackParamType)>)>
                                    &alarmOperationsCallback) -> std::shared_ptr<ResponseType>;
    };
} // namespace alarms
