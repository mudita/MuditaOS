#pragma once

namespace db
{
    enum interface
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

constexpr const char *c_str(enum db::interface db)
{
    switch (db) {
    case db::interface::Settings:
        return "Settings";
    case db::interface::SMS:
        return "SMS";
    case db::interface::Contact:
        return "Contact";
    case db::interface::Alarms:
        return "Alarms";
    case db::interface::Notes:
        return "Notes";
    case db::interface::Calllog:
        return "Callog";
    case db::interface::CountryCodes:
        return "CountryCodes";
    };
    return "";
}
