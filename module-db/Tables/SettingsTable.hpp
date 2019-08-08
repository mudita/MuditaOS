
/*
 * @file SettingsTable.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 05.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_SETTINGSTABLE_HPP
#define PUREPHONE_SETTINGSTABLE_HPP

#include "Table.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

struct SettingsTableRow{
    uint32_t ID;
    bool timeFormat12;
    bool timeAuto;
    bool brightnessAuto;
    uint32_t brightnessLevel;
    uint32_t fontSize;
    SettingsPinMode pinMode;
    uint32_t pinDays;
    uint32_t pinDaysLeft;
    std::string pin1;
    std::string pin2;
    uint32_t    activeSIM;
    std::string     networkOperator;
    uint32_t lockPassHash;
    SettingsLanguage language;
};

enum class SettingsTableFields{
    Dummy
};

class SettingsTable : public Table<SettingsTableRow,SettingsTableFields> {
public:

    SettingsTable(Database* db);
    virtual ~SettingsTable();

    bool Create() override final;
    bool Update(SettingsTableRow entry) override final;
    SettingsTableRow GetByID(uint32_t id) override final;




    bool Add(SettingsTableRow entry) override final;
    bool RemoveByID(uint32_t id) override final;
    std::vector<SettingsTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<SettingsTableRow>
    GetLimitOffsetByField(uint32_t offset, uint32_t limit, SettingsTableFields field,const char* str) override final;
    uint32_t GetCount() override final;
    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final;

private:

    const char* createTableQuery =
            "CREATE TABLE IF NOT EXISTS settings("
            "_id INTEGER PRIMARY KEY, "
            "time_format_12 INTEGER DEFAULT 0, "
            "time_auto INTEGER DEFAULT 1, "
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
            "language INTEGER DEFAULT 1 "
            ");";

    const char* settingsInitialization =
    		"INSERT OR IGNORE INTO settings (_id, time_format_12, time_auto, brightness_auto, brightness_level, bigger_font, pin_mode, pin_days, pin_days_left, pin1_string,pin2_string, active_sim,network_operator,lock_pass_hash,language) VALUES(1,0,1,1,0,0,1,0,0,'','',1,'',0,1)";

};


#endif //PUREPHONE_SETTINGSTABLE_HPP
