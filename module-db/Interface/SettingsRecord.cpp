// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsRecord.hpp"

SettingsRecord::ActiveSim SettingsRecord::to(const uint32_t sim)
{
    if (sim <= static_cast<uint32_t>(ActiveSim::SIM2)) {
        return ActiveSim(sim);
    }
    return ActiveSim::NONE;
}

uint32_t SettingsRecord::from(const ActiveSim sim)
{
    return static_cast<uint32_t>(sim);
}

SettingsRecordInterface::SettingsRecordInterface(SettingsDB *db) : settingsDB(db)
{}

SettingsRecordInterface::~SettingsRecordInterface()
{}

SettingsRecord SettingsRecordInterface::GetByID(uint32_t id)
{

    auto rec = settingsDB->settings.getById(1);

    return SettingsRecord{.dbID            = rec.ID,
                          .timeFormat12    = rec.timeFormat12,
                          .timeAuto        = rec.timeAuto,
                          .timeDateFormat  = rec.timeDateFormat,
                          .brightnessAuto  = rec.brightnessAuto,
                          .brightnessLevel = rec.brightnessLevel,
                          .fontSize        = rec.fontSize,
                          .pinMode         = rec.pinMode,
                          .pinDays         = rec.pinDays,
                          .pinDaysLeft     = rec.pinDaysLeft,
                          .pin1            = rec.pin1,
                          .pin2            = rec.pin2,
                          .activeSIM       = SettingsRecord::to(rec.activeSIM),
                          .networkOperator = rec.networkOperator,
                          .lockPassHash    = rec.lockPassHash,
                          .lockTime        = rec.lockTime,
                          .displayLanguage = rec.displayLanguage,
                          .inputLanguage   = rec.inputLanguage};
}

bool SettingsRecordInterface::Update(const SettingsRecord &rec)
{

    return settingsDB->settings.update(SettingsTableRow{.ID              = rec.dbID,
                                                        .timeFormat12    = rec.timeFormat12,
                                                        .timeAuto        = rec.timeAuto,
                                                        .timeDateFormat  = rec.timeDateFormat,
                                                        .brightnessAuto  = rec.brightnessAuto,
                                                        .brightnessLevel = rec.brightnessLevel,
                                                        .fontSize        = rec.fontSize,
                                                        .pinMode         = rec.pinMode,
                                                        .pinDays         = rec.pinDays,
                                                        .pinDaysLeft     = rec.pinDaysLeft,
                                                        .pin1            = rec.pin1,
                                                        .pin2            = rec.pin2,
                                                        .activeSIM       = SettingsRecord::from(rec.activeSIM),
                                                        .networkOperator = rec.networkOperator,
                                                        .lockPassHash    = rec.lockPassHash,
                                                        .lockTime        = rec.lockTime,
                                                        .displayLanguage = rec.displayLanguage,
                                                        .inputLanguage   = rec.inputLanguage});
}
