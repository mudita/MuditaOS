
/*
 * @file SettingsTable.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 05.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "SettingsTable.hpp"

SettingsTable::SettingsTable(Database *db) : Table(db)
{}

SettingsTable::~SettingsTable()
{}

bool SettingsTable::create()
{
    bool ret = true;
    ret      = db->execute(createTableQuery);
    if (!ret) {
        return false;
    }

    ret = db->execute(settingsInitialization);
    return ret;
}

SettingsTableRow SettingsTable::getById(uint32_t id)
{

    auto retQuery = db->query("SELECT * FROM settings WHERE _id= 1;");

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return SettingsTableRow();
    }

    return SettingsTableRow{
        (*retQuery)[0].GetUInt32(),                                 // ID
        (*retQuery)[1].GetBool(),                                   // timeFormat12
        (*retQuery)[2].GetBool(),                                   // timeAuto
        (*retQuery)[3].GetBool(),                                   // timeDateFormat
        (*retQuery)[4].GetBool(),                                   // brightnessAuto
        (*retQuery)[5].GetUInt32(),                                 // brightnessLevel
        (*retQuery)[6].GetUInt32(),                                 // fontSize
        static_cast<SettingsPinMode>((*retQuery)[7].GetUInt32()),   // pinMode
        (*retQuery)[8].GetUInt32(),                                 // pinDays
        (*retQuery)[9].GetUInt32(),                                 // pinDaysLeft
        (*retQuery)[10].GetString(),                                // pin1
        (*retQuery)[11].GetString(),                                // pin2
        (*retQuery)[12].GetUInt32(),                                // activeSIM
        (*retQuery)[13].GetString(),                                // networkOperator
        (*retQuery)[14].GetUInt32(),                                // lockPassHash
        (*retQuery)[15].GetUInt32(),                                // lockTime
        static_cast<SettingsLanguage>((*retQuery)[16].GetUInt32()), // language

    };
}

bool SettingsTable::update(SettingsTableRow entry)
{
    return db->execute("UPDATE settings SET time_format_12 = %lu, time_auto = %lu, time_date_format = %lu "
                       ",brightness_auto = %lu, brightness_level = %lu, "
                       "bigger_font = %lu, pin_mode =%lu, pin_days = %lu ,pin_days_left = %lu, pin1_string = '%q', "
                       "pin2_string = '%q', active_sim = %lu, "
                       "network_operator = '%q', lock_pass_hash = %lu, lock_time = %lu, language = %lu WHERE _id=1;",
                       entry.timeFormat12,
                       entry.timeAuto,
                       entry.timeDateFormat,
                       entry.brightnessAuto,
                       entry.brightnessLevel,
                       entry.fontSize,
                       entry.pinMode,
                       entry.pinDays,
                       entry.pinDaysLeft,
                       entry.pin1.c_str(),
                       entry.pin2.c_str(),
                       entry.activeSIM,
                       entry.networkOperator.c_str(),
                       entry.lockPassHash,
                       entry.lockTime,
                       entry.language);
}

bool SettingsTable::add(SettingsTableRow entry)
{
    // dummy
    return true;
}

bool SettingsTable::removeById(uint32_t id)
{
    return true;
}

std::vector<SettingsTableRow> SettingsTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    return std::vector<SettingsTableRow>{};
}

std::vector<SettingsTableRow> SettingsTable::getLimitOffsetByField(uint32_t offset,
                                                                   uint32_t limit,
                                                                   SettingsTableFields field,
                                                                   const char *str)
{
    return std::vector<SettingsTableRow>{};
}

uint32_t SettingsTable::count()
{
    return 0;
}

uint32_t SettingsTable::countByFieldId(const char *field, uint32_t id)
{
    return 0;
}
