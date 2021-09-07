// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <time/dateCommon.hpp>

#include <json11.hpp>

#include <memory>

/**
 * Icalendar Parser complies with:
 * @version RFC5545
 * @link https://tools.ietf.org/html/rfc5545
 */

class Duration
{
    uint32_t week = 0, day = 0, hour = 0, minute = 0;

  public:
    bool isValid = true;
    Duration(uint32_t week = 0, uint32_t day = 0, uint32_t hour = 0, uint32_t minute = 0);
    explicit Duration(const std::string &property);

    [[nodiscard]] auto getDurationInMinutes() const -> uint32_t;
    [[nodiscard]] auto getDurationString() const -> std::string;
};

enum class Action
{
    none,
    audio,
    display,
    procedure
};

class Alarm
{
    Duration trigger;
    Action action = Action::none;

  public:
    bool isValid = true;
    Alarm()      = default;
    explicit Alarm(Duration &beforeEvent, Action action = Action::none);

    void setAction(const std::string &action);
    void setTrigger(const std::string &duration);

    [[nodiscard]] auto getTriggerValue() const -> Duration;
    [[nodiscard]] auto getActionValue() const -> Action;

    [[nodiscard]] auto getTriggerString() const -> std::string;
    [[nodiscard]] auto getActionString() const -> std::string;
};

enum class Frequency
{
    never,
    daily,
    weekly,
    monthly,
    yearly

};

class RecurrenceRule
{
    Frequency frequency = Frequency::never;
    uint32_t count      = 0;
    uint32_t interval   = 0;

  public:
    bool isValid     = true;
    RecurrenceRule() = default;
    RecurrenceRule(Frequency freq, uint32_t count, uint32_t interval);

    void setFrequency(const std::string &property);
    void setCount(const std::string &property);
    void setInterval(const std::string &property);

    [[nodiscard]] auto getFrequencyValue() const -> Frequency;
    [[nodiscard]] auto getCountValue() const -> uint32_t;
    [[nodiscard]] auto getIntervalValue() const -> uint32_t;

    [[nodiscard]] auto getFrequencyString() const -> std::string;
    [[nodiscard]] auto getCountString() const -> std::string;
    [[nodiscard]] auto getIntervalString() const -> std::string;
};

class Event
{
    std::string uid;
    std::string summary;
    TimePoint dtstart;
    TimePoint dtend;

    auto isDate(const std::string &dt) -> bool;
    auto isTime(const std::string &dt) -> bool;
    auto validateDT(const std::string &dt) -> bool;
    auto validateUID(const std::string &uid) -> bool;

    [[nodiscard]] auto getDateFromIcalFormat(const std::string &icalDateTime) const -> std::string;
    [[nodiscard]] auto getYearFromIcalDate(const std::string &icalDate) const -> std::string;
    [[nodiscard]] auto getMonthFromIcalDate(const std::string &icalDate) const -> std::string;
    [[nodiscard]] auto getDayFromIcalDate(const std::string &icalDate) const -> std::string;

    [[nodiscard]] auto getTimeFromIcalFormat(const std::string &icalDateTime) const -> std::string;
    [[nodiscard]] auto getHourFromIcalTime(const std::string &icalTime) const -> std::string;
    [[nodiscard]] auto getMinutesFromIcalTime(const std::string &icalTime) const -> std::string;
    [[nodiscard]] auto getSecondsFromIcalTime(const std::string &icalTime) const -> std::string;

    [[nodiscard]] auto dateStringFrom(const std::string &icalDate) const -> std::string;
    [[nodiscard]] auto timeStringFrom(const std::string &icalTime) const -> std::string;

    [[nodiscard]] auto TimePointFromIcalDate(const std::string &icalDateTime) const -> TimePoint;
    [[nodiscard]] auto TimePointToIcalDate(const TimePoint &tp) const -> std::string;

  public:
    bool isValid = true;
    Event()      = default;
    Event(const std::string &summary, TimePoint from, TimePoint till, const std::string &uid);

    void setUID(const std::string &property);
    void setSummary(const std::string &property);
    void setDTStart(const std::string &property);
    void setDTEnd(const std::string &property);

    [[nodiscard]] auto getUID() const -> std::string;
    [[nodiscard]] auto getSummary() const -> std::string;
    [[nodiscard]] auto getDTStartTimePoint() const -> TimePoint;
    [[nodiscard]] auto getDTEndTimePoint() const -> TimePoint;

    [[nodiscard]] auto getDTStartString() const -> std::string;
    [[nodiscard]] auto getDTEndString() const -> std::string;
};

struct ICalEvent
{
    Event event;
    Alarm alarm;
    RecurrenceRule rrule;
};

class ParserICS
{
    std::string icsData;

    void parseFrom(const Event &event);
    void parseFrom(const Alarm &alarm);
    void parseFrom(const RecurrenceRule &rrule);
    void parseFrom(const ICalEvent &icalEvent);

    auto startsWith(const std::string &line, const char *text) const -> bool;
    auto getProperty(const std::string &line) const -> std::string;
    auto getSubProperty(const std::string &Line, const char *propertyName) const -> std::string;

    void eventPropertiesFromLine(ICalEvent &icalEvent, const std::string &line) const;
    void rrulePropertiesFromLine(ICalEvent &icalEvent, const std::string &line) const;
    void alarmPropertiesFromLine(ICalEvent &icalEvent, std::istringstream &input, std::string &line) const;

  public:
    ParserICS() = default;
    void importEvents(std::vector<ICalEvent> events);
    [[nodiscard]] auto exportEvents() const -> std::vector<ICalEvent>;
    void loadData(const std::string &data);

    [[nodiscard]] auto getIcsData() const -> std::string
    {
        return icsData;
    }
};
