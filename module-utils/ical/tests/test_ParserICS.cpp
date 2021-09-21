// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "ParserICS.hpp"

TEST_CASE("Testing Duration class object", "getDurationString method")
{

    SECTION("Test regular week values")
    {
        int test_values[]             = {1, 2, 4, 52, 123};
        std::string expected_values[] = {"1W", "2W", "4W", "52W", "123W"};
        int index                     = 0;
        for (auto value : test_values) {
            auto duration = Duration(value, 0, 0, 0);
            CHECK(duration.getDurationString() == expected_values[index]);
            index++;
        }
    }

    SECTION("Test regular day values")
    {
        int test_values[]             = {1, 2, 3, 7, 30, 333};
        std::string expected_values[] = {"1D", "2D", "3D", "7D", "30D", "333D"};
        int index                     = 0;
        for (auto value : test_values) {
            auto duration = Duration(0, value, 0, 0);
            CHECK(duration.getDurationString() == expected_values[index]);
            index++;
        }
    }

    SECTION("Test regular hour values")
    {
        int test_values[]             = {1, 2, 3, 8, 24, 999};
        std::string expected_values[] = {"T1H", "T2H", "T3H", "T8H", "T24H", "T999H"};
        int index                     = 0;
        for (auto value : test_values) {
            auto duration = Duration(0, 0, value, 0);
            CHECK(duration.getDurationString() == expected_values[index]);
            index++;
        }
    }

    SECTION("Test regular minutes values")
    {
        int test_values[]             = {1, 2, 5, 8, 60, 941683};
        std::string expected_values[] = {"T1M", "T2M", "T5M", "T8M", "T60M", "T941683M"};
        int index                     = 0;
        for (auto value : test_values) {
            auto duration = Duration(0, 0, 0, value);
            CHECK(duration.getDurationString() == expected_values[index]);
            index++;
        }
    }

    SECTION("Test regular mixed week and day values")
    {
        int week_test_value        = 12;
        int day_test_value         = 34;
        std::string expected_value = "12W34D";

        auto duration = Duration(week_test_value, day_test_value, 0, 0);
        CHECK(duration.getDurationString() == expected_value);
    }

    SECTION("Test regular mixed hours and minutes values")
    {
        int hours_test_value       = 3;
        int minutes_test_value     = 5;
        std::string expected_value = "T3H5M";

        auto duration = Duration(0, 0, hours_test_value, minutes_test_value);
        CHECK(duration.getDurationString() == expected_value);
    }

    SECTION("Test regular mixed weeks and minutes values")
    {
        int week_test_value        = 4;
        int day_test_value         = 0;
        int hours_test_value       = 0;
        int minutes_test_value     = 5;
        std::string expected_value = "4WT5M";

        auto duration = Duration(week_test_value, day_test_value, hours_test_value, minutes_test_value);
        CHECK(duration.getDurationString() == expected_value);
    }

    SECTION("Test regular mixed all values")
    {
        int week_test_value        = 1;
        int day_test_value         = 0;
        int hours_test_value       = 8;
        int minutes_test_value     = 23;
        std::string expected_value = "1WT8H23M";

        auto duration = Duration(week_test_value, day_test_value, hours_test_value, minutes_test_value);
        CHECK(duration.getDurationString() == expected_value);
    }

    SECTION("Test getDurationInMinutes() method")
    {
        int week_test_value    = 2;
        int day_test_value     = 1;
        int hours_test_value   = 2;
        int minutes_test_value = 30;
        uint32_t time_in_minutes =
            week_test_value * 7 * 24 * 60 + day_test_value * 24 * 60 + hours_test_value * 60 + minutes_test_value;

        auto duration = Duration(week_test_value, day_test_value, hours_test_value, minutes_test_value);
        CHECK(duration.getDurationInMinutes() == time_in_minutes);
    }

    SECTION("Test Duration(string) constructor")
    {

        std::string duration_value = "P1WT8H23M";
        std::string expected_value = "1WT8H23M";
        auto duration              = Duration(duration_value);
        CHECK(duration.getDurationString() == expected_value);

        std::string duration_value2 = "-P1W1DT8H23M";
        std::string expected_value2 = "1W1DT8H23M";
        auto duration2              = Duration(duration_value2);
        CHECK(duration2.getDurationString() == expected_value2);

        std::string duration_value3 = "-PT8H23M";
        std::string expected_value3 = "T8H23M";
        auto duration3              = Duration(duration_value3);
        CHECK(duration3.getDurationString() == expected_value3);

        std::string duration_value4 = "PT8H23M";
        std::string expected_value4 = "T8H23M";
        auto duration4              = Duration(duration_value4);
        CHECK(duration4.getDurationString() == expected_value4);
    }

    SECTION("Test Duration(string) constructor invalid")
    {
        std::string duration_value = "";
        std::string expected_value = "T0M";
        auto duration              = Duration(duration_value);
        CHECK(duration.getDurationString() == expected_value);

        std::string duration_value1 = "DTSTART:20201020T1536";
        std::string expected_value1 = "T0M";
        auto duration1              = Duration(duration_value1);
        CHECK(duration1.getDurationString() == expected_value1);
    }
}

TEST_CASE("Testing Alarm class object", "")
{

    SECTION("Test regural trigger and action audio")
    {
        auto action                        = Action::audio;
        auto duration                      = Duration(0, 0, 0, 5);
        std::string expected_action_value  = "AUDIO";
        std::string expected_trigger_value = "-PT5M";

        auto alarm = Alarm(duration, action);
        CHECK(alarm.getActionString() == expected_action_value);
        CHECK(alarm.getTriggerString() == expected_trigger_value);
    }

    SECTION("Test regural trigger and action audio")
    {
        auto action                        = Action::procedure;
        auto duration                      = Duration(0, 0, 1, 0);
        std::string expected_action_value  = "PROCEDURE";
        std::string expected_trigger_value = "-PT1H";

        auto alarm = Alarm(duration, action);
        CHECK(alarm.getActionString() == expected_action_value);
        CHECK(alarm.getTriggerString() == expected_trigger_value);
    }

    SECTION("Test regural trigger and action display")
    {
        auto action                        = Action::display;
        auto duration                      = Duration(0, 1, 1, 0);
        std::string expected_action_value  = "DISPLAY";
        std::string expected_trigger_value = "-P1DT1H";

        auto alarm = Alarm(duration, action);
        CHECK(alarm.getActionString() == expected_action_value);
        CHECK(alarm.getTriggerString() == expected_trigger_value);
    }

    SECTION("Test invalid trigger and action")
    {
        auto invalid_action                = Action::invalid;
        auto invalid_duration              = Duration(0, 0, 0, 0);
        std::string expected_action_value  = "";
        std::string expected_trigger_value = "-PT0M";

        auto alarm = Alarm(invalid_duration, invalid_action);
        CHECK(alarm.getActionString() == expected_action_value);
        CHECK(alarm.getTriggerString() == expected_trigger_value);
    }
}

TEST_CASE("Testing RecurrenceRule class object", "")
{

    SECTION("Test frequency daily, count 7 value")
    {
        auto freq                            = Frequency::daily;
        auto rrule                           = RecurrenceRule(freq, 7, 1);
        std::string expected_frequency_value = "DAILY";
        std::string expected_count_value     = "7";
        std::string expected_interval_value  = "1";
        CHECK(rrule.getFrequencyString() == expected_frequency_value);
        CHECK(rrule.getCountString() == expected_count_value);
        CHECK(rrule.getIntervalString() == expected_interval_value);
    }

    SECTION("Test frequency weekly, count 4 value")
    {
        auto freq                            = Frequency::weekly;
        auto rrule                           = RecurrenceRule(freq, 4, 1);
        std::string expected_frequency_value = "WEEKLY";
        std::string expected_count_value     = "4";
        std::string expected_interval_value  = "1";
        CHECK(rrule.getFrequencyString() == expected_frequency_value);
        CHECK(rrule.getCountString() == expected_count_value);
        CHECK(rrule.getIntervalString() == expected_interval_value);
    }

    SECTION("Test frequency biweekly, count 4 value")
    {
        auto freq                            = Frequency::weekly;
        auto rrule                           = RecurrenceRule(freq, 4, 2);
        std::string expected_frequency_value = "WEEKLY";
        std::string expected_count_value     = "4";
        std::string expected_interval_value  = "2";
        CHECK(rrule.getFrequencyString() == expected_frequency_value);
        CHECK(rrule.getCountString() == expected_count_value);
        CHECK(rrule.getIntervalString() == expected_interval_value);
    }

    SECTION("Test frequency monthly, count 12 value")
    {
        auto freq                            = Frequency::monthly;
        auto rrule                           = RecurrenceRule(freq, 12, 1);
        std::string expected_frequency_value = "MONTHLY";
        std::string expected_count_value     = "12";
        std::string expected_interval_value  = "1";
        CHECK(rrule.getFrequencyString() == expected_frequency_value);
        CHECK(rrule.getCountString() == expected_count_value);
        CHECK(rrule.getIntervalString() == expected_interval_value);
    }

    SECTION("Test frequency yearly, count 4 value")
    {
        auto freq                            = Frequency::yearly;
        auto rrule                           = RecurrenceRule(freq, 4, 1);
        std::string expected_frequency_value = "YEARLY";
        std::string expected_count_value     = "4";
        std::string expected_interval_value  = "1";
        CHECK(rrule.getFrequencyString() == expected_frequency_value);
        CHECK(rrule.getCountString() == expected_count_value);
        CHECK(rrule.getIntervalString() == expected_interval_value);
    }

    SECTION("Test frequency invalid")
    {
        auto freq                            = Frequency::invalid;
        auto rrule                           = RecurrenceRule(freq, 1, 1);
        std::string expected_frequency_value = "";
        std::string expected_count_value     = "";
        std::string expected_interval_value  = "";
        CHECK(rrule.getFrequencyString() == expected_frequency_value);
        CHECK(rrule.getCountString() == expected_count_value);
        CHECK(rrule.getIntervalString() == expected_interval_value);
    }

    SECTION("Test count invalid")
    {
        auto freq                            = Frequency::daily;
        auto rrule                           = RecurrenceRule(freq, 1, 0);
        std::string expected_frequency_value = "";
        std::string expected_count_value     = "";
        std::string expected_interval_value  = "";
        CHECK(rrule.getFrequencyString() == expected_frequency_value);
        CHECK(rrule.getCountString() == expected_count_value);
        CHECK(rrule.getIntervalString() == expected_interval_value);
    }

    SECTION("Test interval invalid")
    {
        auto freq                            = Frequency::yearly;
        auto rrule                           = RecurrenceRule(freq, 0, 1);
        std::string expected_frequency_value = "";
        std::string expected_count_value     = "";
        std::string expected_interval_value  = "";
        CHECK(rrule.getFrequencyString() == expected_frequency_value);
        CHECK(rrule.getCountString() == expected_count_value);
        CHECK(rrule.getIntervalString() == expected_interval_value);
    }
}

TEST_CASE("Testing Event class object", "")
{

    SECTION("Test get DTStart and DTEnd in TimePoint value")
    {
        auto summary = "test";
        auto DTstart = TimePointFromString("2020-11-05 15:00:00");
        auto DTend   = TimePointFromString("2020-11-05 16:00:00");

        auto event = Event(summary, DTstart, DTend, "test_uid");

        std::string expected_summary_value = "test";
        std::string expected_DTstart_value = "20201105T150000";
        std::string expected_DTend_value   = "20201105T160000";
        CHECK(event.getSummary() == expected_summary_value);
        CHECK(event.getDTStartString() == expected_DTstart_value);
        CHECK(event.getDTEndString() == expected_DTend_value);

        auto dtstart_timepoint              = event.getDTStartTimePoint();
        auto dtend_timepoint                = event.getDTEndTimePoint();
        std::string expected_dtstart_string = "2020-11-05 15:00:00";
        std::string expected_dtend_string   = "2020-11-05 16:00:00";
        CHECK(TimePointToString(dtstart_timepoint) == expected_dtstart_string);
        CHECK(TimePointToString(dtend_timepoint) == expected_dtend_string);
    }

    SECTION("Test event with invalid timepoints values")
    {
        auto summary = "invalid";
        auto DTstart = TimePointFromString("2020-11-05 15:00");
        auto DTend   = TimePointFromString("2020-11 16:00:00");

        auto event = Event(summary, DTstart, DTend, "test_uid");

        std::string expected_summary_value = "invalid";
        std::string expected_DTstart_value = "19700101T000000";
        std::string expected_DTend_value   = "19700101T000000";
        CHECK(event.getSummary() == expected_summary_value);
        CHECK(event.getDTStartString() == expected_DTstart_value);
        CHECK(event.getDTEndString() == expected_DTend_value);

        auto dtstart_timepoint              = event.getDTStartTimePoint();
        auto dtend_timepoint                = event.getDTEndTimePoint();
        std::string expected_dtstart_string = "1970-01-01 00:00:00";
        std::string expected_dtend_string   = "1970-01-01 00:00:00";
        CHECK(TimePointToString(dtstart_timepoint) == expected_dtstart_string);
        CHECK(TimePointToString(dtend_timepoint) == expected_dtend_string);
    }
}

TEST_CASE("Testing ParserICS class methods", "")
{
    SECTION("Test importEvents and exportEvents")
    {
        /// Event1
        auto summary = "test";
        auto DTstart = TimePointFromString("2020-11-05 15:00:00");
        auto DTend   = TimePointFromString("2020-11-05 16:00:00");
        auto UID     = createUID();
        auto event   = Event(summary, DTstart, DTend, UID);

        auto freq  = Frequency::weekly;
        auto rrule = RecurrenceRule(freq, 4, 2);

        auto action     = Action::audio;
        auto duration   = Duration(0, 0, 0, 5);
        auto alarm      = Alarm(duration, action);
        auto icalEvent1 = ICalEvent{event, alarm, rrule};

        /// Event2
        auto summary2 = "test2";
        auto DTstart2 = TimePointFromString("2019-09-05 10:00:00");
        auto DTend2   = TimePointFromString("2019-09-05 14:30:00");
        auto UID2     = createUID();
        auto event2   = Event(summary2, DTstart2, DTend2, UID2);

        ICalEvent icalEvent2;
        icalEvent2.event = event2;

        /// Event3
        auto summary3 = "test3";
        auto DTstart3 = TimePointFromString("2021-01-01 00:00:00");
        auto DTend3   = TimePointFromString("2021-01-01 10:45:00");
        auto UID3     = createUID();
        auto event3   = Event(summary3, DTstart3, DTend3, UID3);

        auto freq3  = Frequency::daily;
        auto rrule3 = RecurrenceRule(freq3, 7, 1);

        ICalEvent icalEvent3;
        icalEvent3.event = event3;
        icalEvent3.rrule = rrule3;

        /// check
        auto IcalEventsINPUT = std::vector<ICalEvent>();
        IcalEventsINPUT.push_back(icalEvent1);
        IcalEventsINPUT.push_back(icalEvent2);
        IcalEventsINPUT.push_back(icalEvent3);

        auto parser = ParserICS();
        parser.importEvents(IcalEventsINPUT);

        [[maybe_unused]] auto icsFormat = parser.getIcsData();

        std::string expected_uid[]     = {UID, UID2, UID3};
        std::string expected_summary[] = {summary, summary2, summary3};
        std::string expected_DTstart[] = {"20201105T150000", "20190905T100000", "20210101T000000"};
        std::string expected_DTend[]   = {"20201105T160000", "20190905T143000", "20210101T104500"};

        auto icalEventsOUTPUT = parser.exportEvents();

        CHECK(icalEventsOUTPUT[0].event.getUID() == expected_uid[0]);
        CHECK(icalEventsOUTPUT[0].event.getSummary() == expected_summary[0]);
        CHECK(icalEventsOUTPUT[0].event.getDTStartString() == expected_DTstart[0]);
        CHECK(icalEventsOUTPUT[0].event.getDTEndString() == expected_DTend[0]);

        CHECK(icalEventsOUTPUT[1].event.getUID() == expected_uid[1]);
        CHECK(icalEventsOUTPUT[1].event.getSummary() == expected_summary[1]);
        CHECK(icalEventsOUTPUT[1].event.getDTStartString() == expected_DTstart[1]);
        CHECK(icalEventsOUTPUT[1].event.getDTEndString() == expected_DTend[1]);

        CHECK(icalEventsOUTPUT[2].event.getUID() == expected_uid[2]);
        CHECK(icalEventsOUTPUT[2].event.getSummary() == expected_summary[2]);
        CHECK(icalEventsOUTPUT[2].event.getDTStartString() == expected_DTstart[2]);
        CHECK(icalEventsOUTPUT[2].event.getDTEndString() == expected_DTend[2]);

        CHECK(icalEventsOUTPUT[0].rrule.getFrequencyString() == "WEEKLY");
        CHECK(icalEventsOUTPUT[0].rrule.getCountString() == "4");
        CHECK(icalEventsOUTPUT[0].rrule.getIntervalString() == "2");

        CHECK(icalEventsOUTPUT[2].rrule.getFrequencyString() == "DAILY");
        CHECK(icalEventsOUTPUT[2].rrule.getCountString() == "7");
        CHECK(icalEventsOUTPUT[2].rrule.getIntervalString() == "1");

        CHECK(icalEventsOUTPUT[0].alarm.getActionString() == "AUDIO");
        CHECK(icalEventsOUTPUT[0].alarm.getTriggerString() == "-PT5M");
    }

    SECTION("Test importEvents and exportEvents invalid")
    {
        /// Event1
        auto event = Event();

        auto freq  = Frequency::weekly;
        auto rrule = RecurrenceRule(freq, 4, 2);

        auto action   = Action::audio;
        auto duration = Duration(0, 0, 0, 5);
        auto alarm    = Alarm(duration, action);

        auto icalEvent1 = ICalEvent{event, alarm, rrule};

        /// Event2
        auto summary2   = "test2";
        auto DTstart2   = TimePointFromString("2019-09-05 10:00:00");
        auto DTend2     = TimePointFromString("2019-09-05 14:30:00");
        auto UID2       = createUID();
        auto event2     = Event(summary2, DTstart2, DTend2, UID2);
        auto alarm2     = Alarm();
        auto rrule2     = RecurrenceRule();
        auto icalEvent2 = ICalEvent{event2, alarm2, rrule2};

        /// check
        auto IcalEventsINPUT = std::vector<ICalEvent>();
        IcalEventsINPUT.push_back(icalEvent1);
        IcalEventsINPUT.push_back(icalEvent2);

        auto parser = ParserICS();
        parser.importEvents(std::move(IcalEventsINPUT));

        [[maybe_unused]] auto icsFormat = parser.getIcsData();

        std::string expected_uid     = UID2;
        std::string expected_summary = summary2;
        std::string expected_DTstart = "20190905T100000";
        std::string expected_DTend   = "20190905T143000";

        auto icalEventsOUTPUT = parser.exportEvents();

        CHECK(icalEventsOUTPUT[0].event.getUID() == expected_uid);
        CHECK(icalEventsOUTPUT[0].event.getSummary() == expected_summary);
        CHECK(icalEventsOUTPUT[0].event.getDTStartString() == expected_DTstart);
        CHECK(icalEventsOUTPUT[0].event.getDTEndString() == expected_DTend);

        CHECK(icalEventsOUTPUT[0].rrule.getFrequencyString() == "");
        CHECK(icalEventsOUTPUT[0].rrule.getCountString() == "");
        CHECK(icalEventsOUTPUT[0].rrule.getIntervalString() == "");

        CHECK(icalEventsOUTPUT[0].alarm.getActionString() == "");
        CHECK(icalEventsOUTPUT[0].alarm.getTriggerString() == "-PT0M");
    }
}
