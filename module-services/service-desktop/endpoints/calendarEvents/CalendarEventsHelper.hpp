// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common/Query.hpp"
#include "EventsRecord.hpp"
#include "Service/Service.hpp"
#include "Service/Common.hpp"
#include <memory>
#include <service-db/DBServiceAPI.hpp>
#include <endpoints/Endpoint.hpp>
#include <endpoints/Context.hpp>
#include <endpoints/DBHelper.hpp>

#include <module-utils/ical/ParserICS.hpp>

namespace parserFSM
{

    class CalendarEventsHelper : public DBHelper
    {
        [[nodiscard]] auto frequencyFromCustomRepeat(Repeat repeat) const -> Frequency;
        [[nodiscard]] auto recurrenceRuleFrom(Repeat repeat) const -> RecurrenceRule;
        [[nodiscard]] auto alarmFrom(Reminder reminder) const -> Alarm;
        [[nodiscard]] auto icalEventFrom(const EventsRecord &record) const -> ICalEvent;

        [[nodiscard]] auto frequencyToCustomRepeat(Frequency freq) const -> uint32_t;
        [[nodiscard]] auto repeatFrom(RecurrenceRule &rrule) const -> Repeat;
        [[nodiscard]] auto eventsRecordFrom(ICalEvent &icalEvent) const -> EventsRecord;

        [[nodiscard]] auto eventJsonObjectFrom(const EventsRecord &record) const -> json11::Json;
        [[nodiscard]] auto ICalEventFromJson(const json11::Json &eventObj) const -> ICalEvent;
        [[nodiscard]] auto isICalEventValid(const ICalEvent &event) const -> bool;

      public:
        explicit CalendarEventsHelper(sys::Service *_ownerServicePtr) : DBHelper(_ownerServicePtr)
        {}

        auto createDBEntry(Context &context) -> sys::ReturnCodes override;
        auto requestDataFromDB(Context &context) -> sys::ReturnCodes override;
        auto updateDBEntry(Context &context) -> sys::ReturnCodes override;
        auto deleteDBEntry(Context &context) -> sys::ReturnCodes override;
    };
} // namespace parserFSM
