#pragma once

namespace db
{
    enum DB
    {
        Settings,
        SMS,
        Contact,
        Alarms,
        Notes,
        Calllog,
        CountryCodes
    };
}

constexpr const char *c_str(enum db::DB db)
{
    switch (db) {
    case db::DB::Settings:
        return "Settings";
    case db::DB::SMS:
        return "SMS";
    case db::DB::Contact:
        return "Contact";
    case db::DB::Alarms:
        return "Alarms";
    case db::DB::Notes:
        return "Notes";
    case db::DB::Calllog:
        return "Callog";
    case db::DB::CountryCodes:
        return "CountryCodes";
    };
    return "";
}
