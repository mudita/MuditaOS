#pragma once

#include <memory>

namespace db
{

    class Query;
    class QueryResult;

    class Interface
    {
      public:
        virtual std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query);

        enum class Name
        {
            Settings,
            SMS,
            SMSThread,
            SMSTemplate,
            Contact,
            Alarms,
            Notes,
            Calllog,
            CountryCodes,
            Notifications,
            Events,
            Settings_v2
        };
    };
}; // namespace db

constexpr const char *c_str(enum db::Interface::Name db)
{
    switch (db) {
    case db::Interface::Name::Settings:
        return "Settings";
    case db::Interface::Name::SMS:
        return "SMS";
    case db::Interface::Name::SMSThread:
        return "SMSThread";
    case db::Interface::Name::SMSTemplate:
        return "SMSTemplate";
    case db::Interface::Name::Contact:
        return "Contact";
    case db::Interface::Name::Alarms:
        return "Alarms";
    case db::Interface::Name::Notes:
        return "Notes";
    case db::Interface::Name::Calllog:
        return "Callog";
    case db::Interface::Name::CountryCodes:
        return "CountryCodes";
    case db::Interface::Name::Notifications:
        return "Notifications";
    case db::Interface::Name::Events:
        return "Events";
    case db::Interface::Name::Settings_v2:
        return "Settings_v2";
    };
    return "";
}
