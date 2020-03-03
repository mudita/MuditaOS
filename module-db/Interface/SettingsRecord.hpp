
/*
 * @file SettingsRecord.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 06.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "Record.hpp"
#include "utf8/UTF8.hpp"
#include "../Common/Common.hpp"
#include "../Databases/SettingsDB.hpp"

struct SettingsRecord{
    uint32_t dbID;
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
    uint32_t lockTime;
    SettingsLanguage language;
};

enum class SettingsRecordField{
    Dummy
};

class SettingsRecordInterface : public RecordInterface<SettingsRecord,SettingsRecordField> {
public:

    SettingsRecordInterface(SettingsDB* db);
    ~SettingsRecordInterface();

    bool Update(const SettingsRecord& rec) override final;
    SettingsRecord GetByID(uint32_t id) override final;

private:
    SettingsDB* settingsDB;
};
