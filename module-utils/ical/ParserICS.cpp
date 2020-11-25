// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ParserICS.hpp"
//#include <exception>

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
    } // namespace rrule

    namespace duration
    {
        constexpr inline auto minutesInHour = 60;
        constexpr inline auto minutesInDay  = 24 * minutesInHour;
        constexpr inline auto minutesInWeek = 7 * minutesInDay;
    } // namespace duration
} // namespace ical

Duration::Duration(uint32_t week, uint32_t day, uint32_t hour, uint32_t minute)
{
    this->week   = week;
    this->day    = day;
    this->hour   = hour;
    this->minute = minute;
}

Duration::Duration(const std::string &property)
{
    uint32_t i = 0;
    if (property.empty() || (property[0] != 'P' && property[1] != 'P')) {
        LOG_ERROR("Duration constructor: Invalid format provided: %s", property.c_str());
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
                }
            }
            catch (std::exception &e) {
                LOG_DEBUG("Duration conversion from string to int failed with exception:%s", e.what());
            }
        }
        ++i;
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
    if (week) {
        durationString += std::to_string(week) + 'W';
    }
    if (day) {
        durationString += std::to_string(day) + 'D';
    }
    if (hour || minute) {
        durationString += 'T';
    }
    if (hour) {
        durationString += std::to_string(hour) + 'H';
    }
    if (minute) {
        durationString += std::to_string(minute) + 'M';
    }
    if (week == 0 && day == 0 && hour == 0 && minute == 0) {
        durationString += "T0M";
    }
    return durationString;
}

Alarm::Alarm(Duration &beforeEvent, Action action)
{
    if (beforeEvent.getDurationString().empty()) {
        LOG_ERROR("Got empty duration value");
        return;
    }
    this->trigger = beforeEvent;
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
    else {
        this->action = Action::invalid;
    }
}

void Alarm::setTrigger(const std::string &duration)
{
    this->trigger = Duration(duration);
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
    return ical::alarm::value::before + trigger.getDurationString();
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
    case Action::invalid: {
        LOG_ERROR("Alarm with no action");
        return "";
    }
    }
    return "";
}

RecurrenceRule::RecurrenceRule(Frequency freq, uint32_t count, uint32_t interval)
{
    if (count == 0 || interval == 0) {
        LOG_ERROR("Invalid count or interval value!");
        return;
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
    else {
        LOG_ERROR("Invalid frequency!");
        this->frequency = Frequency::invalid;
    }
}
void RecurrenceRule::setCount(const std::string &property)
{
    try {
        this->count = stoi(property);
    }
    catch (...) {
        LOG_ERROR("Count value not conversionable to int!");
    }
}

void RecurrenceRule::setInterval(const std::string &property)
{
    try {
        this->interval = stoi(property);
    }
    catch (...) {
        LOG_ERROR("Interval value not conversionable to int!");
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
    case Frequency::invalid: {
        LOG_ERROR("Frequency invalid");
        return "";
    }
    }
    return frequencyStr;
}

auto RecurrenceRule::getCountString() const -> std::string
{
    if (this->frequency == Frequency::invalid) {
        LOG_ERROR("Frequency value is invalid!");
        return "";
    }
    return std::to_string(this->count);
}

auto RecurrenceRule::getIntervalString() const -> std::string
{
    if (this->frequency == Frequency::invalid) {
        LOG_ERROR("Frequency value is invalid!");
        return "";
    }

    return std::to_string(this->interval);
}

auto Event::getDateFromIcalFormat(const std::string &icalDateTime) const -> std::string
{
    return icalDateTime.substr(0, icalDateTime.find_first_of('T'));
}

auto Event::getTimeFromIcalFormat(const std::string &icalDateTime) const -> std::string
{
    return icalDateTime.substr(icalDateTime.find_first_of('T') + 1);
}

auto Event::dateStringFrom(const std::string &icalDate) const -> std::string
{
    return icalDate.substr(0, yearDigitsNumb) + "-" + icalDate.substr(yearDigitsNumb, monthDigitsNumb) + "-" +
           icalDate.substr(yearDigitsNumb + monthDigitsNumb, dayDigitsNumb);
}

auto Event::timeStringFrom(const std::string &icalTime) const -> std::string
{
    return icalTime.substr(0, HourDigitsNumb) + ":" + icalTime.substr(HourDigitsNumb, MinDigitsNumb) + ":" +
           icalTime.substr(HourDigitsNumb + MinDigitsNumb, SecDigitsNumb);
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

Event::Event(const std::string &summary, const TimePoint from, TimePoint till, const std::string &uid)
{
    this->summary = summary;
    this->dtstart = from;
    this->dtend   = till;
    this->uid     = uid;
}

void Event::setUID(const std::string &property)
{
    this->uid = property;
}
void Event::setSummary(const std::string &property)
{
    this->summary = property;
}
void Event::setDTStart(const std::string &property)
{
    this->dtstart = TimePointFromIcalDate(property);
}
void Event::setDTEnd(const std::string &property)
{
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
