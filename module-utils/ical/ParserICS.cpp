// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ParserICS.hpp"
#include <cmath>
#include <time/time_date_validation.hpp>

namespace ical
{
    constexpr inline auto begin = "BEGIN:VCALENDAR";

    namespace with_linefeed
    {
        constexpr inline auto begin   = "BEGIN:VCALENDAR\r\n";
        constexpr inline auto version = "VERSION:2.0\r\n";
        constexpr inline auto end     = "END:VCALENDAR\r\n";
    } // namespace with_linefeed

    namespace event
    {
        namespace with_linefeed
        {
            constexpr inline auto begin = "BEGIN:VEVENT\r\n";
            constexpr inline auto end   = "END:VEVENT\r\n";
        } // namespace with_linefeed
        constexpr inline auto begin = "BEGIN:VEVENT";
        constexpr inline auto end   = "END:VEVENT";

        constexpr inline auto uid     = "UID:";
        constexpr inline auto summary = "SUMMARY:";
        constexpr inline auto dtstart = "DTSTART:";
        constexpr inline auto dtend   = "DTEND:";
    } // namespace event

    namespace alarm
    {
        namespace with_linefeed
        {
            constexpr inline auto begin = "BEGIN:VALARM\r\n";
            constexpr inline auto end   = "END:VALARM\r\n";
        } // namespace with_linefeed
        constexpr inline auto begin = "BEGIN:VALARM";
        constexpr inline auto end   = "END:VALARM";

        constexpr inline auto trigger = "TRIGGER:";
        constexpr inline auto action  = "ACTION:";

        namespace value
        {
            // Time, 0, Minutes
            namespace action
            {
                constexpr inline auto display   = "DISPLAY";
                constexpr inline auto procedure = "PROCEDURE";
                constexpr inline auto audio     = "AUDIO";
            } // namespace action
            constexpr inline auto trigger_invalid = "T0M";
            constexpr inline auto before          = "-P";
            constexpr inline auto after           = "P";
        } // namespace value
    }     // namespace alarm

    namespace rrule
    {
        constexpr inline auto rrule = "RRULE:";
        constexpr inline auto freq  = "FREQ=";
        namespace frequency
        {
            constexpr inline auto daily   = "DAILY";
            constexpr inline auto weekly  = "WEEKLY";
            constexpr inline auto monthly = "MONTHLY";
            constexpr inline auto yearly  = "YEARLY";
        } // namespace frequency
        constexpr inline auto count    = "COUNT=";
        constexpr inline auto interval = "INTERVAL=";

        constexpr inline auto max_interval = 2;
    } // namespace rrule

    namespace duration
    {
        constexpr inline auto minutesInHour = 60;
        constexpr inline auto minutesInDay  = 24 * minutesInHour;
        constexpr inline auto minutesInWeek = 7 * minutesInDay;

        namespace minutes
        {
            constexpr inline auto never_happens_value = 0xFFFF;
        }
    } // namespace duration
    constexpr inline auto date_char_length = yearDigitsNumb + monthDigitsNumb + dayDigitsNumb;
    constexpr inline auto time_char_length = HourDigitsNumb + MinDigitsNumb + SecDigitsNumb;
    constexpr inline auto dt_char_length   = date_char_length + 1 + time_char_length;
} // namespace ical

Duration::Duration(uint32_t week, uint32_t day, uint32_t hour, uint32_t minute)
{
    if (minute == ical::duration::minutes::never_happens_value) {
        this->minute = ical::duration::minutes::never_happens_value;
        return;
    }
    this->week   = week;
    this->day    = day;
    this->hour   = hour;
    this->minute = minute;

    if (!utils::time::validateTime(hour, minute, false)) {
        isValid = false;
    }
}

Duration::Duration(const std::string &property)
{
    if (property.empty()) {
        LOG_DEBUG("Duration is empty. Event with no Alarm!");
        this->minute = ical::duration::minutes::never_happens_value;
        return;
    }
    uint32_t i = 0;
    if ((property[0] != 'P' && property[1] != 'P')) {
        LOG_ERROR("Duration constructor: Invalid format provided: %s", property.c_str());
        isValid = false;
        return;
    }
    while (property[i] != '\0' && i < property.length()) {
        if (isdigit(property[i])) {
            std::string value;
            while (isdigit(property[i])) {
                value += property[i];
                ++i;
            }
            try {
                switch (property[i]) {
                case 'W':
                    this->week = stoi(value);
                    break;
                case 'D':
                    this->day = stoi(value);
                    break;
                case 'H':
                    this->hour = stoi(value);
                    break;
                case 'M':
                    this->minute = stoi(value);
                    break;
                default:
                    LOG_ERROR("Wrong duration unit value format");
                    isValid = false;
                    break;
                }
            }
            catch (std::exception &e) {
                LOG_DEBUG("Duration conversion from string to int failed with exception:%s", e.what());
                isValid = false;
            }
        }
        ++i;
    }

    if (!utils::time::validateTime(hour, minute, false)) {
        LOG_ERROR("Duration time value is invalid");
        isValid = false;
    }
}

auto Duration::getDurationInMinutes() const -> uint32_t
{
    auto weeksMinutes = week * ical::duration::minutesInWeek;
    auto daysMinutes  = day * ical::duration::minutesInDay;
    auto hoursMinutes = hour * ical::duration::minutesInHour;

    return weeksMinutes + daysMinutes + hoursMinutes + minute;
}

auto Duration::getDurationString() const -> std::string
{
    std::string durationString;
    if (minute == ical::duration::minutes::never_happens_value) {
        return durationString;
    }
    if (week) {
        durationString += utils::to_string(week) + 'W';
    }
    if (day) {
        durationString += utils::to_string(day) + 'D';
    }
    if (hour || minute) {
        durationString += 'T';
    }
    if (hour) {
        durationString += utils::to_string(hour) + 'H';
    }
    if (minute) {
        durationString += utils::to_string(minute) + 'M';
    }
    if (week == 0 && day == 0 && hour == 0 && minute == 0) {
        durationString += "T0M";
    }
    return durationString;
}

Alarm::Alarm(Duration &timeBeforeEvent, Action action)
{
    if (!timeBeforeEvent.isValid) {
        LOG_ERROR("Duration format is invalid");
        isValid = false;
        return;
    }
    this->trigger = timeBeforeEvent;
    this->action  = action;
}

void Alarm::setAction(const std::string &action)
{
    if (action == ical::alarm::value::action::display) {
        this->action = Action::display;
    }
    else if (action == ical::alarm::value::action::procedure) {
        this->action = Action::procedure;
    }
    else if (action == ical::alarm::value::action::audio) {
        this->action = Action::audio;
    }
    else if (action.empty()) {
        LOG_DEBUG("Alarm with no action");
        this->action = Action::none;
    }
    else {
        LOG_ERROR("Alarm action invalid");
        isValid = false;
    }
}

void Alarm::setTrigger(const std::string &duration)
{
    auto timeBeforeEvent = Duration(duration);
    if (!timeBeforeEvent.isValid) {
        LOG_ERROR("Duration format is invalid");
        isValid = false;
        return;
    }
    this->trigger = timeBeforeEvent;
}

auto Alarm::getTriggerValue() const -> Duration
{
    return this->trigger;
}

auto Alarm::getActionValue() const -> Action
{
    return this->action;
}

auto Alarm::getTriggerString() const -> std::string
{
    auto durationString = trigger.getDurationString();
    if (durationString.empty()) {
        return durationString;
    }
    return ical::alarm::value::before + durationString;
}

auto Alarm::getActionString() const -> std::string
{
    std::string actionStr = ical::alarm::action;
    switch (this->action) {
    case Action::audio: {
        return ical::alarm::value::action::audio;
    }
    case Action::display: {
        return ical::alarm::value::action::display;
    }
    case Action::procedure: {
        return ical::alarm::value::action::procedure;
    }
    case Action::none: {
        LOG_DEBUG("Alarm with no action");
        return "";
    }
    }
    return "";
}

RecurrenceRule::RecurrenceRule(Frequency freq, uint32_t count, uint32_t interval)
{
    if ((count == 0 || interval == 0) && freq != Frequency::never) {
        LOG_ERROR("Invalid rrule format!");
        isValid = false;
        return;
    }
    if (interval > ical::rrule::max_interval) {
        LOG_ERROR("Invalid interval value!");
        isValid = false;
    }
    this->frequency = freq;
    this->count     = count;
    this->interval  = interval;
}

void RecurrenceRule::setFrequency(const std::string &property)
{
    if (property == ical::rrule::frequency::daily) {
        this->frequency = Frequency::daily;
    }
    else if (property == ical::rrule::frequency::weekly) {
        this->frequency = Frequency::weekly;
    }
    else if (property == ical::rrule::frequency::monthly) {
        this->frequency = Frequency::monthly;
    }
    else if (property == ical::rrule::frequency::yearly) {
        this->frequency = Frequency::yearly;
    }
    else if (property.empty()) {
        this->frequency = Frequency::never;
    }
    else {
        LOG_ERROR("Invalid frequency!");
        isValid = false;
    }
}
void RecurrenceRule::setCount(const std::string &property)
{
    try {
        this->count = stoi(property);
    }
    catch (...) {
        LOG_ERROR("Count value is not an integer!");
        isValid = false;
    }
}

void RecurrenceRule::setInterval(const std::string &property)
{
    try {
        this->interval = stoi(property);
    }
    catch (...) {
        LOG_ERROR("Interval value not conversionable to int!");
        isValid = false;
    }
}

auto RecurrenceRule::getFrequencyValue() const -> Frequency
{
    return this->frequency;
}

auto RecurrenceRule::getCountValue() const -> uint32_t
{
    return this->count;
}

auto RecurrenceRule::getIntervalValue() const -> uint32_t
{
    return this->interval;
}

auto RecurrenceRule::getFrequencyString() const -> std::string
{
    std::string frequencyStr = ical::rrule::freq;
    switch (this->frequency) {
    case Frequency::daily: {
        frequencyStr = ical::rrule::frequency::daily;
        break;
    }
    case Frequency::weekly: {
        frequencyStr = ical::rrule::frequency::weekly;
        break;
    }
    case Frequency::monthly: {
        frequencyStr = ical::rrule::frequency::monthly;
        break;
    }
    case Frequency::yearly: {
        frequencyStr = ical::rrule::frequency::yearly;
        break;
    }
    case Frequency::never: {
        LOG_DEBUG("Frequency never");
        return "";
    }
    }
    return frequencyStr;
}

auto RecurrenceRule::getCountString() const -> std::string
{
    return utils::to_string(this->count);
}

auto RecurrenceRule::getIntervalString() const -> std::string
{
    return utils::to_string(this->interval);
}

auto Event::getDateFromIcalFormat(const std::string &icalDateTime) const -> std::string
{
    return icalDateTime.substr(0, icalDateTime.find_first_of('T'));
}

auto Event::getYearFromIcalDate(const std::string &icalDate) const -> std::string
{
    return icalDate.substr(0, yearDigitsNumb);
}

auto Event::getMonthFromIcalDate(const std::string &icalDate) const -> std::string
{
    return icalDate.substr(yearDigitsNumb, monthDigitsNumb);
}

auto Event::getDayFromIcalDate(const std::string &icalDate) const -> std::string
{
    return icalDate.substr(yearDigitsNumb + monthDigitsNumb, dayDigitsNumb);
}

auto Event::getTimeFromIcalFormat(const std::string &icalDateTime) const -> std::string
{
    return icalDateTime.substr(icalDateTime.find_first_of('T') + 1);
}

auto Event::getHourFromIcalTime(const std::string &icalTime) const -> std::string
{
    return icalTime.substr(0, HourDigitsNumb);
}

auto Event::getMinutesFromIcalTime(const std::string &icalTime) const -> std::string
{
    return icalTime.substr(HourDigitsNumb, MinDigitsNumb);
}

auto Event::getSecondsFromIcalTime(const std::string &icalTime) const -> std::string
{
    return icalTime.substr(HourDigitsNumb + MinDigitsNumb, SecDigitsNumb);
}

auto Event::dateStringFrom(const std::string &icalDate) const -> std::string
{
    return getYearFromIcalDate(icalDate) + "-" + getMonthFromIcalDate(icalDate) + "-" + getDayFromIcalDate(icalDate);
}

auto Event::timeStringFrom(const std::string &icalTime) const -> std::string
{
    return getHourFromIcalTime(icalTime) + ":" + getMinutesFromIcalTime(icalTime) + ":" +
           getSecondsFromIcalTime(icalTime);
}

auto Event::TimePointFromIcalDate(const std::string &icalDateTime) const -> TimePoint
{
    std::string icalDate(getDateFromIcalFormat(icalDateTime));
    std::string icalTime(getTimeFromIcalFormat(icalDateTime));

    std::string date(dateStringFrom(icalDate));
    std::string time(timeStringFrom(icalTime));

    std::string dateTime(date + " " + time);

    return TimePointFromString(dateTime.c_str());
}

auto Event::TimePointToIcalDate(const TimePoint &tp) const -> std::string
{
    constexpr uint32_t bufferLimit = 16;
    auto time                      = TimePointToTimeT(tp);
    auto utcTime                   = gmtime(&time);
    char Buffer[bufferLimit];
    strftime(Buffer, bufferLimit, "%Y%m%dT%H%M%S", utcTime);
    std::string IcalDate = Buffer;
    return IcalDate;
}

auto Event::isDate(const std::string &date) -> bool
{
    return utils::time::validateDate(getDayFromIcalDate(date), getMonthFromIcalDate(date), getYearFromIcalDate(date));
}

auto Event::isTime(const std::string &time) -> bool
{
    [[maybe_unused]] uint32_t seconds;
    try {
        seconds = stoi(getSecondsFromIcalTime(time));
    }
    catch (...) {
        LOG_ERROR("Seconds value is not an integer!");
        return false;
    }
    return utils::time::validateTime(getHourFromIcalTime(time), getMinutesFromIcalTime(time), false);
}

auto Event::validateDT(const std::string &dt) -> bool
{
    uint32_t dateTimeSize;
    if (dt.find_first_of('Z') == ical::date_char_length + ical::time_char_length) {
        dateTimeSize = ical::dt_char_length + 1;
    }
    else {
        dateTimeSize = ical::dt_char_length;
    }
    if (dt.size() != dateTimeSize) {
        LOG_ERROR("Date time length is invalid");
        return false;
    }

    auto separatorIndex = dt.find_first_of('T');
    LOG_DEBUG("Separator index = %d", (int)separatorIndex);
    if (separatorIndex != (yearDigitsNumb + monthDigitsNumb + dayDigitsNumb)) {
        LOG_ERROR("Date time separator is invalid");
        return false;
    }

    auto date = getDateFromIcalFormat(dt);
    auto time = getTimeFromIcalFormat(dt);

    if (!isDate(date)) {
        LOG_ERROR("Date is invalid");
        return false;
    }

    if (!isTime(time)) {
        LOG_ERROR("Time is invalid");
        return false;
    }

    return true;
}

auto Event::validateUID(const std::string &UID) -> bool
{
    auto DTimestamp = UID.substr(0, UID.find_first_of('-'));
    auto id         = UID.substr(UID.find_first_of('-') + 1);

    try {
        stoi(id);
    }
    catch (...) {
        LOG_ERROR("UID value is not an integer!");
        return false;
    }

    return validateDT(DTimestamp);
}

Event::Event(const std::string &summary, const TimePoint from, TimePoint till, const std::string &uid)
{
    if (summary.empty()) {
        isValid = false;
    }
    if (!validateUID(uid) && !uid.empty()) {
        isValid = false;
    }
    this->uid     = uid;
    this->summary = summary;
    this->dtstart = from;
    this->dtend   = till;
}

void Event::setUID(const std::string &property)
{
    if (!validateUID(property) && !property.empty()) {
        LOG_ERROR("UID invalid format");
        isValid = false;
    }
    this->uid = property;
}
void Event::setSummary(const std::string &property)
{
    if (property.empty()) {
        isValid = false;
    }
    this->summary = property;
}
void Event::setDTStart(const std::string &property)
{
    if (!validateDT(property)) {
        isValid = false;
    }
    this->dtstart = TimePointFromIcalDate(property);
}
void Event::setDTEnd(const std::string &property)
{
    if (!validateDT(property)) {
        isValid = false;
    }
    this->dtend = TimePointFromIcalDate(property);
}

auto Event::getUID() const -> std::string
{
    return uid;
}

auto Event::getSummary() const -> std::string
{
    return summary;
}

auto Event::getDTStartTimePoint() const -> TimePoint
{
    return dtstart;
}

auto Event::getDTEndTimePoint() const -> TimePoint
{
    return dtend;
}

auto Event::getDTStartString() const -> std::string
{
    return TimePointToIcalDate(dtstart);
}
auto Event::getDTEndString() const -> std::string
{
    return TimePointToIcalDate(dtend);
}

void ParserICS::parseFrom(const Event &event)
{
    icsData += ical::event::uid + event.getUID() + "\r\n";
    icsData += ical::event::summary + event.getSummary() + "\r\n";
    icsData += ical::event::dtstart + event.getDTStartString() + "\r\n";
    icsData += ical::event::dtend + event.getDTEndString() + "\r\n";
}

void ParserICS::parseFrom(const Alarm &alarm)
{
    auto triggerValue  = alarm.getTriggerString();
    std::string prefix = ical::alarm::value::before;
    if (triggerValue == (prefix + ical::alarm::value::trigger_invalid)) {
        LOG_DEBUG("Got Alarm with empty trigger");
        return;
    }
    icsData += ical::alarm::with_linefeed::begin;
    icsData += ical::alarm::trigger + triggerValue + "\r\n";
    icsData += ical::alarm::action + alarm.getActionString() + "\r\n";
    icsData += ical::alarm::with_linefeed::end;
}

void ParserICS::parseFrom(const RecurrenceRule &rrule)
{
    auto freq = rrule.getFrequencyString();
    if (!freq.empty()) {
        icsData += std::string(ical::rrule::rrule) + ical::rrule::freq + freq + ";" + ical::rrule::count +
                   rrule.getCountString() + ";" + ical::rrule::interval + rrule.getIntervalString() + "\r\n";
    }
    else {
        LOG_DEBUG("Empty Alarm provided");
    }
}

void ParserICS::parseFrom(const ICalEvent &icalEvent)
{
    if (icalEvent.event.getUID() == "") {
        LOG_ERROR("Empty event provided to ical parser");
        return;
    }
    icsData += ical::event::with_linefeed::begin;
    parseFrom(icalEvent.event);
    parseFrom(icalEvent.rrule);
    parseFrom(icalEvent.alarm);
    icsData += ical::event::with_linefeed::end;
}

void ParserICS::eventPropertiesFromLine(ICalEvent &icalEvent, const std::string &line) const
{
    if (startsWith(line, ical::event::begin)) {
        icalEvent.event = Event();
    }
    else if (startsWith(line, ical::event::uid)) {
        icalEvent.event.setUID(getProperty(line));
    }
    else if (startsWith(line, ical::event::summary)) {
        icalEvent.event.setSummary(getProperty(line));
    }
    else if (startsWith(line, ical::event::dtstart)) {
        icalEvent.event.setDTStart(getProperty(line));
    }
    else if (startsWith(line, ical::event::dtend)) {
        icalEvent.event.setDTEnd(getProperty(line));
    }
}

void ParserICS::rrulePropertiesFromLine(ICalEvent &icalEvent, const std::string &line) const
{
    icalEvent.rrule    = RecurrenceRule();
    auto formattedLine = getProperty(line);
    icalEvent.rrule.setFrequency(getSubProperty(formattedLine, ical::rrule::freq));
    icalEvent.rrule.setCount(getSubProperty(formattedLine, ical::rrule::count));
    icalEvent.rrule.setInterval(getSubProperty(formattedLine, ical::rrule::interval));
}

void ParserICS::alarmPropertiesFromLine(ICalEvent &icalEvent, std::istringstream &input, std::string &line) const
{
    icalEvent.alarm = Alarm();

    while (getline(input, line)) {
        if (startsWith(line, ical::alarm::trigger)) {
            icalEvent.alarm.setTrigger(getProperty(line));
        }
        else if (startsWith(line, ical::alarm::action)) {
            icalEvent.alarm.setAction(getProperty(line));
        }
        else if (startsWith(line, ical::alarm::end)) {
            break;
        }
    }
}

auto ParserICS::startsWith(const std::string &line, const char *text) const -> bool
{
    return (line.find(text) == 0);
}

auto ParserICS::getProperty(const std::string &line) const -> std::string
{
    std::string Temp    = line.substr(line.find_first_of(':') + 1);
    unsigned int Length = Temp.length();
    if (Length > 0 && Temp[Length - 1] == '\r')
        Temp.resize(Length - 1);
    return Temp;
}

auto ParserICS::getSubProperty(const std::string &Line, const char *propertyName) const -> std::string
{
    size_t pos = Line.find(propertyName);
    if (pos == std::string::npos)
        return "";
    pos += strlen(propertyName);
    return Line.substr(pos, Line.find_first_of(';', pos) - pos);
}

void ParserICS::importEvents(std::vector<ICalEvent> events)
{
    this->icsData = ical::with_linefeed::begin;
    this->icsData += +ical::with_linefeed::version;

    for (auto event : events) {
        parseFrom(event);
    }

    this->icsData += ical::with_linefeed::end;
}

auto ParserICS::exportEvents() const -> std::vector<ICalEvent>
{
    std::vector<ICalEvent> events;
    ICalEvent icalEvent;

    std::istringstream input;
    input.str(icsData);
    std::string line;

    while (getline(input, line)) {
        eventPropertiesFromLine(icalEvent, line);

        if (startsWith(line, ical::rrule::rrule)) {
            rrulePropertiesFromLine(icalEvent, line);
        }
        else if (startsWith(line, ical::alarm::begin)) {
            alarmPropertiesFromLine(icalEvent, input, line);
        }
        else if (startsWith(line, ical::event::end)) {
            events.push_back(icalEvent);
        }
    }

    return events;
}

void ParserICS::loadData(const std::string &data)
{
    std::istringstream input;
    input.str(data);
    std::string line;

    getline(input, line);
    if (!startsWith(line, ical::begin)) {
        LOG_ERROR("Wrong format");
        return;
    }

    this->icsData = data;
}
