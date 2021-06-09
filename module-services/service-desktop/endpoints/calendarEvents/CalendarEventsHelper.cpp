// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventsRecord.hpp"
#include "CalendarEventsHelper.hpp"
#include "Common/Query.hpp"
#include <parser/ParserUtils.hpp>
#include "Service/Common.hpp"
#include <service-db/DBServiceAPI.hpp>
#include <log.hpp>
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
#include <utility>

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
            const inline auto never                  = Duration(0, 0, 0, 0xFFFF);
        } // namespace duration
    }     // namespace ical

    namespace json::calendar
    {
        constexpr inline auto events = "calendar_events";
        namespace event
        {
            constexpr inline auto vevent = "VEVENT";

            constexpr inline auto uid     = "UID";
            constexpr inline auto summary = "SUMMARY";
            constexpr inline auto dtstart = "DTSTART";
            constexpr inline auto dtend   = "DTEND";

            namespace recurrence_rule
            {
                constexpr inline auto rrule = "RRULE";

                constexpr inline auto frequency = "FREQ";
                constexpr inline auto count     = "COUNT";
                constexpr inline auto interval  = "INTERVAL";
            } // namespace recurrence_rule

            namespace alarm
            {
                constexpr inline auto valarm = "VALARM";

                constexpr inline auto trigger = "TRIGGER";
                constexpr inline auto action  = "ACTION";
            } // namespace alarm

            namespace provider
            {
                constexpr inline auto provider = "provider";

                constexpr inline auto type    = "type";
                constexpr inline auto id      = "id";
                constexpr inline auto iCalUid = "iCalUid";
            } // namespace provider
        }     // namespace event
        constexpr inline auto limit  = "limit";
        constexpr inline auto offset = "offset";

    } // namespace json::calendar
} // namespace parserFSM
using namespace parserFSM;

auto CalendarEventsHelper::isICalEventValid(const ICalEvent &icalEvent) const -> bool
{
    if (!icalEvent.event.isValid) {
        LOG_ERROR("Ical event invalid!");
        return false;
    }
    if (!icalEvent.alarm.isValid) {
        LOG_ERROR("Ical alarm invalid!");
        return false;
    }
    if (!icalEvent.rrule.isValid) {
        LOG_ERROR("Ical recurrence rule invalid!");
        return false;
    }
    return true;
}

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
        auto beforeEvent = ical::duration::never;
        return Alarm(beforeEvent, Action::none);
    }
    case Reminder::five_min_before: {
        auto beforeEvent = ical::duration::five_minutes_before;
        return Alarm(beforeEvent, Action::none);
    }
    case Reminder::fifteen_min_before: {
        auto beforeEvent = ical::duration::fifteen_minutes_before;
        return Alarm(beforeEvent, Action::none);
    }
    case Reminder::thirty_min_before: {
        auto beforeEvent = ical::duration::thirty_minutes_before;
        return Alarm(beforeEvent, Action::none);
    }
    case Reminder::one_hour_before: {
        auto beforeEvent = ical::duration::one_hour_before;
        return Alarm(beforeEvent, Action::none);
    }
    case Reminder::two_hour_before: {
        auto beforeEvent = ical::duration::two_hours_before;
        return Alarm(beforeEvent, Action::none);
    }
    case Reminder::one_day_before: {
        auto beforeEvent = ical::duration::one_day_before;
        return Alarm(beforeEvent, Action::none);
    }
    case Reminder::two_days_before: {
        auto beforeEvent = ical::duration::two_days_before;
        return Alarm(beforeEvent, Action::none);
    }
    case Reminder::one_week_before: {
        auto beforeEvent = ical::duration::one_week_before;
        return Alarm(beforeEvent, Action::none);
    }
    case Reminder::event_time: {
        auto beforeEvent = ical::duration::event_time;
        return Alarm(beforeEvent, Action::none);
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

auto CalendarEventsHelper::eventJsonObjectFrom(const EventsRecord &record) const -> json11::Json
{
    auto icalEvent = icalEventFrom(record);
    if (!isICalEventValid(icalEvent)) {
        LOG_ERROR("Bad event record formatting  (Event UID: %s)", icalEvent.event.getUID().c_str());
    }

    auto rruleObj = json11::Json::object{
        {json::calendar::event::recurrence_rule::frequency, icalEvent.rrule.getFrequencyString().c_str()},
        {json::calendar::event::recurrence_rule::count, icalEvent.rrule.getCountString().c_str()},
        {json::calendar::event::recurrence_rule::interval, icalEvent.rrule.getIntervalString().c_str()}};

    auto alarmObj =
        json11::Json::object{{json::calendar::event::alarm::trigger, icalEvent.alarm.getTriggerString().c_str()},
                             {json::calendar::event::alarm::action, icalEvent.alarm.getActionString().c_str()}};

    auto providerObj =
        json11::Json::object{{json::calendar::event::provider::type, record.provider_type.c_str()},
                             {json::calendar::event::provider::id, record.provider_id.c_str()},
                             {json::calendar::event::provider::iCalUid, record.provider_iCalUid.c_str()}};

    auto eventObj = json11::Json::object{{json::calendar::event::dtend, icalEvent.event.getDTEndString().c_str()},
                                         {json::calendar::event::dtstart, icalEvent.event.getDTStartString().c_str()},
                                         {json::calendar::event::summary, icalEvent.event.getSummary().c_str()},
                                         {json::calendar::event::uid, icalEvent.event.getUID().c_str()},
                                         {json::calendar::event::recurrence_rule::rrule, rruleObj},
                                         {json::calendar::event::alarm::valarm, alarmObj},
                                         {json::calendar::event::provider::provider, providerObj}};

    return eventObj;
}

auto CalendarEventsHelper::requestDataFromDB(Context &context) -> sys::ReturnCodes
{
    try {
        auto &ctx          = dynamic_cast<PagedContext &>(context);
        std::size_t limit  = ctx.getBody()[json::calendar::limit].int_value();
        std::size_t offset = ctx.getBody()[json::calendar::offset].int_value();
        ctx.setRequestedLimit(limit);
        ctx.setRequestedOffset(offset);
        auto query = std::make_unique<db::query::events::GetAllLimited>(offset, std::min(ctx.getPageSize(), limit));

        auto listener = std::make_unique<db::EndpointListenerWithPages>(
            [&](db::QueryResult *result, PagedContext &context) {
                if (auto EventsResult = dynamic_cast<db::query::events::GetAllLimitedResult *>(result)) {
                    auto records        = EventsResult->getResult();
                    uint32_t totalCount = EventsResult->getCountResult();
                    auto parser         = std::make_unique<ParserICS>();
                    std::vector<ICalEvent> icalEvents;
                    context.setTotalCount(totalCount);
                    auto eventsArray = json11::Json::array();

                    for (const auto &rec : records) {
                        auto eventObject = eventJsonObjectFrom(rec);
                        eventsArray.emplace_back(eventObject);
                    }
                    context.setResponseBody(eventsArray);
                    MessageHandler::putToSendQueue(context.createSimpleResponse(json::calendar::events));

                    return true;
                }
                return false;
            },
            ctx);

        query->setQueryListener(std::move(listener));
        DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Events, std::move(query));
    }
    catch (const std::exception &e) {
        LOG_ERROR("%s", e.what());
        return sys::ReturnCodes::Failure;
    }
    return sys::ReturnCodes::Success;
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
    case Frequency::never: {
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

auto CalendarEventsHelper::ICalEventFromJson(const json11::Json &eventObj) const -> ICalEvent
{
    ICalEvent icalEvent;
    icalEvent.event.setUID(eventObj[json::calendar::event::uid].string_value());
    icalEvent.event.setSummary(eventObj[json::calendar::event::summary].string_value());
    icalEvent.event.setDTStart(eventObj[json::calendar::event::dtstart].string_value());
    icalEvent.event.setDTEnd(eventObj[json::calendar::event::dtend].string_value());

    icalEvent.rrule.setFrequency(
        eventObj[json::calendar::event::recurrence_rule::rrule][json::calendar::event::recurrence_rule::frequency]
            .string_value());
    icalEvent.rrule.setCount(
        eventObj[json::calendar::event::recurrence_rule::rrule][json::calendar::event::recurrence_rule::count]
            .string_value());
    icalEvent.rrule.setInterval(
        eventObj[json::calendar::event::recurrence_rule::rrule][json::calendar::event::recurrence_rule::interval]
            .string_value());

    icalEvent.alarm.setTrigger(
        eventObj[json::calendar::event::alarm::valarm][json::calendar::event::alarm::trigger].string_value());
    icalEvent.alarm.setAction(
        eventObj[json::calendar::event::alarm::valarm][json::calendar::event::alarm::action].string_value());

    auto record = eventsRecordFrom(icalEvent);

    return icalEvent;
}

auto CalendarEventsHelper::createDBEntry(Context &context) -> sys::ReturnCodes
{
    const auto eventsJsonObj   = context.getBody();
    const auto eventsJsonArray = eventsJsonObj[json::calendar::events].array_items();
    bool ret                   = true;
    for (const auto &event : eventsJsonArray) {

        auto icalEvent = ICalEventFromJson(event);

        if (!isICalEventValid(icalEvent)) {
            context.setResponseStatus(http::Code::BadRequest);
            MessageHandler::putToSendQueue(context.createSimpleResponse());
            return sys::ReturnCodes::Failure;
        }

        auto record = eventsRecordFrom(icalEvent);
        if (record.UID.empty()) {
            record.UID   = createUID();
            auto jsonObj = json11::Json::object({{json::calendar::event::uid, record.UID}});
            context.setResponseBody(jsonObj);
        }
        else {
            LOG_ERROR("UID should not be recieved in put event endpoint. Recieved UID: %s", record.UID.c_str());
            context.setResponseStatus(http::Code::BadRequest);
            MessageHandler::putToSendQueue(context.createSimpleResponse());
            return sys::ReturnCodes::Failure;
        }

        auto query    = std::make_unique<db::query::events::Add>(record);
        auto listener = std::make_unique<db::EndpointListener>(
            [&](db::QueryResult *result, Context context) {
                if (auto EventResult = dynamic_cast<db::query::events::AddResult *>(result)) {

                    context.setResponseStatus(EventResult->getResult() ? http::Code::OK
                                                                       : http::Code::InternalServerError);

                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                return false;
            },
            context);

        query->setQueryListener(std::move(listener));
        const auto [succeed, _] =
            DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Events, std::move(query));
        ret = ret && succeed;
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
    auto eventsJsonObj = context.getBody();

    bool ret = true;
    for (const auto &event : eventsJsonObj[json::calendar::events].array_items()) {

        auto icalEvent = ICalEventFromJson(event);
        if (!isICalEventValid(icalEvent) || icalEvent.event.getUID().empty()) {
            context.setResponseStatus(http::Code::BadRequest);
            MessageHandler::putToSendQueue(context.createSimpleResponse());
            return sys::ReturnCodes::Failure;
        }

        auto record   = eventsRecordFrom(icalEvent);
        auto query    = std::make_unique<db::query::events::EditICS>(record);
        auto listener = std::make_unique<db::EndpointListener>(
            [](db::QueryResult *result, Context context) {
                if (auto EventResult = dynamic_cast<db::query::events::EditICSResult *>(result)) {
                    context.setResponseStatus(EventResult->getResult() ? http::Code::NoContent
                                                                       : http::Code::InternalServerError);
                    MessageHandler::putToSendQueue(context.createSimpleResponse());
                    return true;
                }
                return false;
            },
            context);

        query->setQueryListener(std::move(listener));
        const auto [succeed, _] =
            DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Events, std::move(query));
        ret = ret && succeed;
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
    auto UID      = context.getBody()[json::calendar::event::uid].string_value();
    auto checkUID = Event();
    checkUID.setUID(UID);
    if (!checkUID.isValid) {
        LOG_ERROR("Wrong UID format. Provided UID: %s", UID.c_str());
        context.setResponseStatus(http::Code::BadRequest);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
        return sys::ReturnCodes::Failure;
    }
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
    auto [ret, _] = DBServiceAPI::GetQuery(ownerServicePtr, db::Interface::Name::Events, std::move(query));

    if (ret) {
        return sys::ReturnCodes::Success;
    }
    else {
        return sys::ReturnCodes::Failure;
    }
}
