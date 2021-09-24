// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <application-alarm-clock/presenter/AlarmRRulePresenter.hpp>

using namespace app::alarmClock;

class PubPresenter : public AlarmRRulePresenter
{
  public:
    PubPresenter() : AlarmRRulePresenter(std::make_shared<AlarmEventRecord>())
    {}

    utl::Day dayToDay(uint32_t day_no)
    {
        return AlarmRRulePresenter::dayToDay(day_no);
    }

    uint8_t dayToDay(utl::Day day)
    {
        return AlarmRRulePresenter::dayToDay(day);
    }
};

TEST_CASE("transformations test")
{
    auto p = PubPresenter();
    REQUIRE(p.dayToDay(uint8_t(rrule::RRule::RRuleWeekday::MONDAY_WEEKDAY) == utl::Mon));
}

TEST_CASE("empty rrule")
{
    std::shared_ptr<AlarmEventRecord> alarm;
    AlarmRRulePresenter presenter(alarm);
    REQUIRE(!presenter.hasRecurrence());
}

/// libcal has days set in format starting with Sunday on 1,
TEST_CASE("recurrence rule get & set")
{
    auto alarm = std::make_shared<AlarmEventRecord>();
    AlarmRRulePresenter presenter(alarm);

    SECTION("no recurrence")
    {
        REQUIRE(!presenter.hasRecurrence());
    }

    SECTION("everyday recurrence")
    {
        alarm->rruleText = "FREQ=WEEKLY;BYDAY=SU,MO,TU,WE,TH,FR,SA;INTERVAL=1";
        REQUIRE(presenter.hasRecurrence());
        REQUIRE(presenter.getOption() == AlarmRRulePresenter::RRuleOptions::Everyday);
        REQUIRE(presenter.getDays().size() == 7);
    }

    SECTION("clearing recurrence")
    {
        alarm->rruleText = "FREQ=WEEKLY;BYDAY=SU,MO,TU,WE,TH,FR,SA;INTERVAL=1";
        presenter.setDays({});
        REQUIRE(presenter.getDays().empty());
    }

    SECTION("changing recurrence")
    {
        alarm->rruleText = "FREQ=WEEKLY;BYDAY=MO,FR;INTERVAL=1";
        presenter.setDays({utl::Tue});
        REQUIRE(presenter.getDays().size() == 1);
        REQUIRE(presenter.getDays().front() == utl::Tue);
    }

    SECTION("changing recurrence to sunday")
    {
        presenter.setDays({utl::Sun});
        REQUIRE(presenter.getDays().size() == 1);
        REQUIRE(presenter.getDays().front() == utl::Sun);
    }
}

TEST_CASE("getOptions")
{
    auto alarm = std::make_shared<AlarmEventRecord>();
    AlarmRRulePresenter presenter(alarm);

    SECTION("nothing")
    {
        REQUIRE(presenter.getOption() == AlarmRRulePresenter::RRuleOptions::Never);
    }

    SECTION("weekdays")
    {
        presenter.setDays({utl::Mon, utl::Tue, utl::Wed, utl::Thu, utl::Fri});
        REQUIRE(presenter.getOption() == AlarmRRulePresenter::RRuleOptions::Weekdays);
    }

    SECTION("week")
    {
        presenter.setDays({utl::Mon, utl::Tue, utl::Wed, utl::Thu, utl::Fri, utl::Sat, utl::Sun});
        REQUIRE(presenter.getOption() == AlarmRRulePresenter::RRuleOptions::Everyday);
    }

    SECTION("custom")
    {
        presenter.setDays({utl::Tue, utl::Wed, utl::Thu, utl::Fri, utl::Sat, utl::Sun});
        REQUIRE(presenter.getOption() == AlarmRRulePresenter::RRuleOptions::Custom);
    }

    SECTION("signle day")
    {
        presenter.setDays({utl::Tue});
        REQUIRE(presenter.getOption() == AlarmRRulePresenter::RRuleOptions::OnDay);
    }
}

TEST_CASE("getCustomDays")
{
    auto alarm = std::make_shared<AlarmEventRecord>();
    AlarmRRulePresenter presenter(alarm);

    std::list<utl::Day> days = {utl::Sun, utl::Wed, utl::Fri};
    presenter.setOption(AlarmRRulePresenter::RRuleOptions::Custom, days);

    REQUIRE(presenter.getCustomDays()[0] == std::pair<std::string, bool>{utils::time::Locale::get_day(utl::Sun), true});
    REQUIRE(presenter.getCustomDays()[1] ==
            std::pair<std::string, bool>{utils::time::Locale::get_day(utl::Mon), false});
    REQUIRE(presenter.getCustomDays()[2] ==
            std::pair<std::string, bool>{utils::time::Locale::get_day(utl::Tue), false});
    REQUIRE(presenter.getCustomDays()[3] == std::pair<std::string, bool>{utils::time::Locale::get_day(utl::Wed), true});
    REQUIRE(presenter.getCustomDays()[4] ==
            std::pair<std::string, bool>{utils::time::Locale::get_day(utl::Thu), false});
    REQUIRE(presenter.getCustomDays()[5] == std::pair<std::string, bool>{utils::time::Locale::get_day(utl::Fri), true});
    REQUIRE(presenter.getCustomDays()[6] ==
            std::pair<std::string, bool>{utils::time::Locale::get_day(utl::Sat), false});
}

TEST_CASE("setOptions")
{
    auto alarm = std::make_shared<AlarmEventRecord>();
    AlarmRRulePresenter presenter(alarm);

    SECTION("nothing")
    {
        presenter.setOption(AlarmRRulePresenter::RRuleOptions::Never);
        REQUIRE(presenter.getDays().empty());
    }

    SECTION("weekdays")
    {
        presenter.setOption(AlarmRRulePresenter::RRuleOptions::Weekdays);
        REQUIRE(presenter.getDays() == std::list<utl::Day>{utl::Mon, utl::Tue, utl::Wed, utl::Thu, utl::Fri});
    }

    SECTION("week")
    {
        presenter.setOption(AlarmRRulePresenter::RRuleOptions::Everyday);
        REQUIRE(presenter.getDays() ==
                std::list<utl::Day>{utl::Sun, utl::Mon, utl::Tue, utl::Wed, utl::Thu, utl::Fri, utl::Sat});
    }

    SECTION("custom")
    {
        std::list<utl::Day> days = {utl::Sun, utl::Wed, utl::Fri};

        presenter.setOption(AlarmRRulePresenter::RRuleOptions::Custom, days);
        REQUIRE(presenter.getDays() == days);
    }
}
