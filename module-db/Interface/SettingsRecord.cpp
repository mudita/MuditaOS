
/*
 * @file SettingsRecord.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 06.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "SettingsRecord.hpp"
#include "../Databases/SettingsDB.hpp"

SettingsRecord SettingsRecordInterface::GetByID(uint32_t id) {
    auto settingsDB = std::make_unique<SettingsDB>();

    auto rec = settingsDB->settings.GetByID(1);

    return SettingsRecord{
            .dbID = rec.ID,
            .timeFormat12 = rec.timeFormat12,
            .timeAuto = rec.timeAuto,
            .brightnessAuto = rec.brightnessAuto,
            .brightnessLevel = rec.brightnessLevel,
            .fontSize = rec.fontSize,
            .pinMode = rec.pinMode,
            .pinDays = rec.pinDays,
            .pinDaysLeft = rec.pinDaysLeft,
            .pin1 = rec.pin1,
            .pin2 = rec.pin2,
            .activeSIM = rec.activeSIM,
            .networkOperator = rec.networkOperator,
            .lockPassHash = rec.lockPassHash,
            .language = rec.language
    };
}

bool SettingsRecordInterface::Update(const SettingsRecord &rec) {
    auto settingsDB = std::make_unique<SettingsDB>();

    return settingsDB->settings.Update(SettingsTableRow{
        .ID = rec.dbID,
        .timeFormat12 = rec.timeFormat12,
        .timeAuto = rec.timeAuto,
        .brightnessAuto = rec.brightnessAuto,
        .brightnessLevel = rec.brightnessLevel,
        .fontSize = rec.fontSize,
        .pinMode = rec.pinMode,
        .pinDays = rec.pinDays,
        .pinDaysLeft = rec.pinDaysLeft,
        .pin1 = rec.pin1,
        .pin2 = rec.pin2,
        .activeSIM = rec.activeSIM,
        .networkOperator = rec.networkOperator,
        .lockPassHash = rec.lockPassHash,
        .language = rec.language
    });
}
