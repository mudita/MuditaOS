// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <application-alarm-clock/presenter/AlarmPresenter.hpp>

using namespace app::alarmClock;

class PubPresenter : public AlarmPresenter
{
  public:
    PubPresenter() : AlarmPresenter(std::make_shared<AlarmEventRecord>())
    {}

    utl::Day dayToDay(uint32_t day_no)
    {
        return AlarmPresenter::dayToDay(day_no);
    }

    uint8_t dayToDay(utl::Day day)
    {
        return AlarmPresenter::dayToDay(day);
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
    AlarmPresenter presenter(alarm);
    REQUIRE(!presenter.hasRecurrence());
}

/// libcal has days set in format starting with Sunday on 1,
TEST_CASE("recurrence rule get & set")
{
    auto alarm = std::make_shared<AlarmEventRecord>();
    AlarmPresenter presenter(alarm);

    SECTION("no recurrence")
    {
        REQUIRE(!presenter.hasRecurrence());
    }

    SECTION("everyday recurrence")
    {
        alarm->rruleText = "FREQ=WEEKLY;BYDAY=SU,MO,TU,WE,TH,FR,SA;INTERVAL=1";
        REQUIRE(presenter.hasRecurrence());
        REQUIRE(presenter.getSpinner() == AlarmPresenter::Spinner::Weekly);
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

TEST_CASE("getSpinner")
{
    auto alarm = std::make_shared<AlarmEventRecord>();
    AlarmPresenter presenter(alarm);

    SECTION("nothing")
    {
        REQUIRE(presenter.getSpinner() == AlarmPresenter::Spinner::Never);
    }

    SECTION("weekdays")
    {
        presenter.setDays({utl::Mon, utl::Tue, utl::Wed, utl::Thu, utl::Fri});
        REQUIRE(presenter.getSpinner() == AlarmPresenter::Spinner::Weekdays);
    }

    SECTION("week")
    {
        presenter.setDays({utl::Mon, utl::Tue, utl::Wed, utl::Thu, utl::Fri, utl::Sat, utl::Sun});
        REQUIRE(presenter.getSpinner() == AlarmPresenter::Spinner::Weekly);
    }

    SECTION("custom")
    {
        presenter.setDays({utl::Tue, utl::Wed, utl::Thu, utl::Fri, utl::Sat, utl::Sun});
        REQUIRE(presenter.getSpinner() == AlarmPresenter::Spinner::Custom);
    }

    SECTION("signle day")
    {
        presenter.setDays({utl::Tue});
        REQUIRE(presenter.getSpinner() == AlarmPresenter::Spinner::OnDay);
    }
}

TEST_CASE("setSpinner")
{
    auto alarm = std::make_shared<AlarmEventRecord>();
    AlarmPresenter presenter(alarm);

    SECTION("nothing")
    {
        presenter.setSpinner(AlarmPresenter::Spinner::Never, nullptr);
        REQUIRE(presenter.getDays().empty());
    }

    SECTION("weekdays")
    {
        presenter.setSpinner(AlarmPresenter::Spinner::Weekdays, nullptr);
        REQUIRE(presenter.getDays() == std::list<utl::Day>{utl::Mon, utl::Tue, utl::Wed, utl::Thu, utl::Fri});
    }

    SECTION("week")
    {
        presenter.setSpinner(AlarmPresenter::Spinner::Weekly, nullptr);
        REQUIRE(presenter.getDays() ==
                std::list<utl::Day>{utl::Sun, utl::Mon, utl::Tue, utl::Wed, utl::Thu, utl::Fri, utl::Sat});
    }
}
