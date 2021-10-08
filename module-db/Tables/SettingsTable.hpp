// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Table.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"
#include <i18n/i18n.hpp>

struct SettingsTableRow
{
    uint32_t ID;
    bool timeFormat12;
    bool timeAuto;
    bool timeDateFormat;
    bool brightnessAuto;
    uint32_t brightnessLevel;
    uint32_t fontSize;
    SettingsPinMode pinMode;
    uint32_t pinDays;
    uint32_t pinDaysLeft;
    std::string pin1;
    std::string pin2;
    uint32_t activeSIM;
    std::string networkOperator;
    uint32_t lockPassHash;
    // time of inactivity of the user after which phone will be automatically blocked.
    uint32_t lockTime;
    Language displayLanguage;
    Language inputLanguage;
};

enum class SettingsTableFields
{
    Dummy
};

class SettingsTable : public Table<SettingsTableRow, SettingsTableFields>
{
  public:
    SettingsTable(Database *db);
    virtual ~SettingsTable();

    bool create() override final;
    bool update(SettingsTableRow entry) override final;
    SettingsTableRow getById(uint32_t id) override final;

    bool add(SettingsTableRow entry) override final;
    bool removeById(uint32_t id) override final;
    std::vector<SettingsTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<SettingsTableRow> getLimitOffsetByField(uint32_t offset,
                                                        uint32_t limit,
                                                        SettingsTableFields field,
                                                        const char *str) override final;
    uint32_t count() override final;
    uint32_t countByFieldId(const char *field, uint32_t id) override final;

  private:
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS settings("
                                   "_id INTEGER PRIMARY KEY, "
                                   "time_format_12 INTEGER DEFAULT 0, "
                                   "time_auto INTEGER DEFAULT 1, "
                                   "time_date_format INTEGER DEFAULT 1, "
                                   "brightness_auto INTEGER DEFAULT 1, "
                                   "brightness_level INTEGER DEFAULT 0, "
                                   "bigger_font INTEGER DEFAULT 0, "
                                   "pin_mode INTEGER DEFAULT 1, "
                                   "pin_days INTEGER DEFAULT 0, "
                                   "pin_days_left INTEGER DEFAULT 0, "
                                   "pin1_string TEXT DEFAULT '', "
                                   "pin2_string TEXT DEFAULT '', "
                                   "active_sim INTEGER DEFAULT 1, "
                                   "network_operator TEXT DEFAULT '', "
                                   "lock_pass_hash INTEGER DEFAULT 0, "
                                   "lock_time INTEGER DEFAULT 30000, "
                                   "display_language TEXT DEFAULT 'English', "
                                   "input_language TEXT DEFAULT 'English' "
                                   ");";

    const char *settingsInitialization = "INSERT OR IGNORE INTO settings (_id, time_format_12, time_auto, "
                                         "time_date_format, brightness_auto, brightness_level, "
                                         "bigger_font, pin_mode, pin_days, pin_days_left, pin1_string,pin2_string, "
                                         "active_sim,network_operator,lock_pass_hash,lock_time, display_language, "
                                         "input_language) "
                                         "VALUES(1,0,1,1,1,0,0,1,0,0,'','',1,'',0,30000,'English','English')";
};
