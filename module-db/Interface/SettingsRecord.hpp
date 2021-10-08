// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Record.hpp"
#include "utf8/UTF8.hpp"
#include "../Common/Common.hpp"
#include "../Databases/SettingsDB.hpp"
#include <i18n/i18n.hpp>

struct SettingsRecord
{
    uint32_t dbID;
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
    enum class ActiveSim : uint32_t
    {
        NONE = 0,
        SIM1 = 1,
        SIM2 = 2,
    } activeSIM = ActiveSim::NONE;
    std::string networkOperator;
    uint32_t lockPassHash;
    uint32_t lockTime;
    Language displayLanguage;
    Language inputLanguage;

    static ActiveSim to(const uint32_t sim);
    static uint32_t from(const ActiveSim sim);
};

enum class SettingsRecordField
{
    Dummy
};

class SettingsRecordInterface : public RecordInterface<SettingsRecord, SettingsRecordField>
{
  public:
    SettingsRecordInterface(SettingsDB *db);
    ~SettingsRecordInterface();

    bool Update(const SettingsRecord &rec) override final;
    SettingsRecord GetByID(uint32_t id) override final;

  private:
    SettingsDB *settingsDB;
};
