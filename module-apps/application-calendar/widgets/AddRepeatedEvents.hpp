#pragma once
#include "Application.hpp"
#include <module-db/Interface/EventsRecord.hpp>

enum class Repeat
{
    Never = 0,
    Daily,
    Weekly,
    TwoWeeks,
    Month,
    Year,
    Custom
};

class AddRepeatedEvents
{
    app::Application *application = nullptr;
    Repeat repeatOption           = Repeat::Never;

  public:
    AddRepeatedEvents(app::Application *app) : application(app){};
    void addDaily(std::shared_ptr<EventsRecord> event);
    void addWeekly(std::shared_ptr<EventsRecord> event);
    void addTwoWeeks(std::shared_ptr<EventsRecord> event);
    void addMonth(std::shared_ptr<EventsRecord> event);
    void addYear(std::shared_ptr<EventsRecord> event);
    void addCustom(std::shared_ptr<EventsRecord> event);

    void addRepeatedEvents(std::shared_ptr<EventsRecord> event);
};
