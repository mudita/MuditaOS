// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventsRecord.hpp"
#include "CalendarEventsHelper.hpp"
#include "Common/Query.hpp"
#include <parser/ParserUtils.hpp>
#include "Service/Common.hpp"
#include <service-db/DBServiceAPI.hpp>
#include "log/log.hpp"
#include "json/json11.hpp"
#include <queries/calendar/QueryEventsGetAll.hpp>
#include <queries/calendar/QueryEventsGet.hpp>
#include <queries/calendar/QueryEventsAdd.hpp>
#include <queries/calendar/QueryEventsEdit.hpp>
#include <queries/calendar/QueryEventsEditICS.hpp>
#include <queries/calendar/QueryEventsRemove.hpp>
#include <queries/calendar/QueryEventsRemoveICS.hpp>
#include <queries/calendar/QueryEventsGetAllLimited.hpp>
#include <variant>

namespace parserFSM
{
    namespace ical
    {
        namespace duration
        {
            const inline auto five_minutes_before    = Duration(0, 0, 0, 5);
            const inline auto fifteen_minutes_before = Duration(0, 0, 0, 15);
            const inline auto thirty_minutes_before  = Duration(0, 0, 0, 30);
            const inline auto one_hour_before        = Duration(0, 0, 1, 0);
            const inline auto two_hours_before       = Duration(0, 0, 2, 0);
            const inline auto one_day_before         = Duration(0, 1, 0, 0);
            const inline auto two_days_before        = Duration(0, 2, 0, 0);
            const inline auto one_week_before        = Duration(1, 0, 0, 0);
            const inline auto event_time             = Duration(0, 0, 0, 0);
        } // namespace duration
    }     // namespace ical

    namespace json::calendar::events
    {
        constexpr inline auto UID         = "UID";
        constexpr inline auto data        = "data";
        constexpr inline auto offset      = "offset";
        constexpr inline auto limit       = "limit";
        constexpr inline auto count       = "count";
        constexpr inline auto total_count = "total_count";

        constexpr inline auto providers = "providers";
        namespace provider
        {
            constexpr inline auto type    = "provider_type";
            constexpr inline auto id      = "provider_id";
            constexpr inline auto iCalUid = "provider_iCalUid";
        } // namespace provider
    }     // namespace json::calendar::events
} // namespace parserFSM
using namespace parserFSM;

auto CalendarEventsHelper::frequencyFromCustomRepeat(Repeat repeat) const -> Frequency
{
    return Frequency(static_cast<uint32_t>(repeat));
}

auto CalendarEventsHelper::recurrenceRuleFrom(Repeat repeat) const -> RecurrenceRule
{
    if (repeat > Repeat::yearly) {
        uint32_t count = 1, interval = 1;
        return RecurrenceRule(frequencyFromCustomRepeat(repeat), count, interval);
    }
    switch (repeat) {
    case Repeat::never: {
        return RecurrenceRule();
    }
    case Repeat::daily: {
        uint32_t count = 7, interval = 1;
        return RecurrenceRule(Frequency::daily, count, interval);
    }
    case Repeat::weekly: {
        uint32_t count = 4, interval = 1;
        return RecurrenceRule(Frequency::weekly, count, interval);
    }
    case Repeat::biweekly: {
        uint32_t count = 4, interval = 2;
        return RecurrenceRule(Frequency::weekly, count, interval);
    }
    case Repeat::monthly: {
        uint32_t count = 12, interval = 1;
        return RecurrenceRule(Frequency::monthly, count, interval);
    }
    case Repeat::yearly: {
        uint32_t count = 4, interval = 1;
        return RecurrenceRule(Frequency::yearly, count, interval);
    }
    }

    return RecurrenceRule();
}

auto CalendarEventsHelper::alarmFrom(Reminder reminder) const -> Alarm
{
    switch (reminder) {
    case Reminder::never: {
        return Alarm();
    }
    case Reminder::five_min_before: {
        auto beforeEvent = ical::duration::five_minutes_before;
        return Alarm(beforeEvent, Action::display);
    }
    case Reminder::fifteen_min_before: {
        auto beforeEvent = ical::duration::fifteen_minutes_before;
        return Alarm(beforeEvent, Action::display);
    }
    case Reminder::thirty_min_before: {
        auto beforeEvent = ical::duration::thirty_minutes_before;
        return Alarm(beforeEvent, Action::display);
    }
    case Reminder::one_hour_before: {
        auto beforeEvent = ical::duration::one_hour_before;
        return Alarm(beforeEvent, Action::display);
    }
    case Reminder::two_hour_before: {
        auto beforeEvent = ical::duration::two_hours_before;
        return Alarm(beforeEvent, Action::display);
    }
    case Reminder::one_day_before: {
        auto beforeEvent = ical::duration::one_day_before;
        return Alarm(beforeEvent, Action::display);
    }
    case Reminder::two_days_before: {
        auto beforeEvent = ical::duration::two_days_before;
        return Alarm(beforeEvent, Action::display);
    }
    case Reminder::one_week_before: {
        auto beforeEvent = ical::duration::one_week_before;
        return Alarm(beforeEvent, Action::display);
    }
    case Reminder::event_time: {
        auto beforeEvent = ical::duration::event_time;
        return Alarm(beforeEvent, Action::display);
    }
    }
    return Alarm();
}

auto CalendarEventsHelper::icalEventFrom(const EventsRecord &record) const -> ICalEvent
{

    auto event = Event(record.title, record.date_from, record.date_till, record.UID);
    auto alarm = alarmFrom(Reminder(record.reminder));
    auto rrule = recurrenceRuleFrom(Repeat(record.repeat));
    return ICalEvent{event, alarm, rrule};
}

auto CalendarEventsHelper::requestDataFromDB(Context &context) -> sys::ReturnCodes
{
    auto obj        = context.getBody();
    uint32_t offset = obj[json::calendar::events::offset].int_value();
    uint32_t limit  = obj[json::calendar::events::limit].int_value();
    auto query      = std::make_unique<db::query::events::GetAllLimited>(offset, limit);

    auto listener = std::make_unique<db::EndpointListener>(
        [&](db::QueryResult *result, Context context) {
            if (auto EventsResult = dynamic_cast<db::query::events::GetAllLimitedResult *>(result)) {
                auto records        = EventsResult->getResult();
                uint32_t totalCount = EventsResult->getCountResult();
                auto parser         = std::make_unique<ParserICS>();
                std::vector<ICalEvent> icalEvents;
                for (auto rec : records) {
                    icalEvents.push_back(icalEventFrom(rec));
                }
                parser->importEvents(icalEvents);
                auto jsonObj =
                    json11::Json::object({{json::calendar::events::data, parser->getIcsData()},
                                          {json::calendar::events::count, std::to_string(records.size())},
                                          {json::calendar::events::total_count, std::to_string(totalCount)}});

                context.setResponseBody(jsonObj);
                MessageHandler::putToSendQueue(context.createSimpleResponse());
                return true;
            }
            return false;
        },
        context);

    query->setQueryListener(std::move(listener));
    auto ret = DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Events, std::move(query));

    if (ret) {
        return sys::ReturnCodes::Success;
    }
    else {
        return sys::ReturnCodes::Failure;
    }
}

auto CalendarEventsHelper::frequencyToCustomRepeat(Frequency freq) const -> uint32_t
{
    return static_cast<uint32_t>(freq);
}

auto CalendarEventsHelper::repeatFrom(RecurrenceRule &rrule) const -> Repeat
{
    auto freq = rrule.getFrequencyValue();
    switch (freq) {
    case Frequency::daily: {
        return Repeat::daily;
    }
    case Frequency::weekly: {
        if (rrule.getIntervalValue() == 1) {
            return Repeat::weekly;
        }
        else if (rrule.getIntervalValue() == 2) {
            return Repeat::biweekly;
        }
        else {
            LOG_ERROR("Interval invalid");
            return Repeat::never;
        }
    }
    case Frequency::monthly: {
        return Repeat::monthly;
    }
    case Frequency::yearly: {
        return Repeat::yearly;
    }
    case Frequency::invalid: {
        LOG_ERROR("Frequency invalid");
        return Repeat::never;
    }
    }
    return Repeat::never;
}

auto CalendarEventsHelper::eventsRecordFrom(ICalEvent &icalEvent) const -> EventsRecord
{

    auto record      = EventsRecord();
    record.UID       = icalEvent.event.getUID();
    record.title     = icalEvent.event.getSummary();
    record.date_from = icalEvent.event.getDTStartTimePoint();
    record.date_till = icalEvent.event.getDTEndTimePoint();
    if (icalEvent.rrule.getFrequencyValue() > Frequency::yearly) {
        record.repeat = frequencyToCustomRepeat(icalEvent.rrule.getFrequencyValue());
    }
    record.repeat   = static_cast<uint32_t>(repeatFrom(icalEvent.rrule));
    record.reminder = icalEvent.alarm.getTriggerValue().getDurationInMinutes();

    return record;
}

auto CalendarEventsHelper::createDBEntry(Context &context) -> sys::ReturnCodes
{
    auto parser = std::make_shared<ParserICS>();
    parser->loadData(context.getBody()[json::calendar::events::data].string_value());
    auto icalEvents = parser->exportEvents();

    bool ret = true;
    for (auto event : icalEvents) {
        auto UID    = event.event.getUID();
        auto record = eventsRecordFrom(event);
        auto query  = std::make_unique<db::query::events::Add>(record);

        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context context) {
                if (auto EventResult = dynamic_cast<db::query::events::AddResult *>(result)) {

                    auto jsonObj = json11::Json::object(
                        {{json::calendar::events::data, parser->getIcsData()}, {json::calendar::events::UID, UID}});

                    context.setResponseBody(jsonObj);
                    context.setResponseStatus(EventResult->getResult() ? http::Code::OK
                                                                       : http::Code::InternalServerError);

                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                return false;
            },
            context);

        query->setQueryListener(std::move(listener));
        ret = ret && DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Events, std::move(query));
    }

    if (ret) {
        return sys::ReturnCodes::Success;
    }
    else {
        return sys::ReturnCodes::Failure;
    }
}

auto CalendarEventsHelper::updateDBEntry(Context &context) -> sys::ReturnCodes
{
    auto parser = std::make_unique<ParserICS>();
    parser->loadData(context.getBody()[json::calendar::events::data].string_value());
    auto icalEvents = parser->exportEvents();

    bool ret = true;
    for (auto event : icalEvents) {

        auto record   = eventsRecordFrom(event);
        auto query    = std::make_unique<db::query::events::EditICS>(record);
        auto listener = std::make_unique<db::EndpointListener>(
            [](db::QueryResult *result, Context context) {
                if (auto EventResult = dynamic_cast<db::query::events::EditICSResult *>(result)) {
                    context.setResponseStatus(EventResult->getResult() ? http::Code::OK
                                                                       : http::Code::InternalServerError);
                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                return false;
            },
            context);

        query->setQueryListener(std::move(listener));
        ret = ret && DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Events, std::move(query));
    }
    if (ret) {
        return sys::ReturnCodes::Success;
    }
    else {
        return sys::ReturnCodes::Failure;
    }
}

auto CalendarEventsHelper::deleteDBEntry(Context &context) -> sys::ReturnCodes
{
    auto UID      = context.getBody()[json::calendar::events::UID].string_value();
    auto query    = std::make_unique<db::query::events::RemoveICS>(UID);
    auto listener = std::make_unique<db::EndpointListener>(
        [=](db::QueryResult *result, Context context) {
            if (auto EventResult = dynamic_cast<db::query::events::RemoveICSResult *>(result)) {
                context.setResponseStatus(EventResult->getResult() ? http::Code::OK : http::Code::InternalServerError);
                MessageHandler::putToSendQueue(context.createSimpleResponse());
                return true;
            }
            return false;
        },
        context);

    query->setQueryListener(std::move(listener));
    auto ret = DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Events, std::move(query));

    if (ret) {
        return sys::ReturnCodes::Success;
    }
    else {
        return sys::ReturnCodes::Failure;
    }
}
