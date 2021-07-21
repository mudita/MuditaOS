// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FactoryData.hpp"

#include <service-db/agents/settings/FactorySettings.hpp>

FactoryData::FactoryData(settings::Settings &settings) : settings(settings)
{}

auto FactoryData::getModel() -> std::string
{
    return settings.getValue(settings::factory::entry_key + std::string("/model"), settings::SettingsScope::Global);
}
auto FactoryData::getCase() -> std::string
{
    return settings.getValue(settings::factory::entry_key + std::string("/case"), ::settings::SettingsScope::Global);
}
auto FactoryData::getSerial() -> std::string
{
    return settings.getValue(settings::factory::entry_key + std::string("/serial"), settings::SettingsScope::Global);
}
auto FactoryData::getBatteryRev() -> std::string
{
    return settings.getValue(settings::factory::entry_key + std::string("/battery_revision"),
                             settings::SettingsScope::Global);
}
auto FactoryData::getPcb(std::string type) -> std::string
{
    std::string full_pcb = "/pcb_" + type + "_version";
    return settings.getValue(settings::factory::entry_key + full_pcb, settings::SettingsScope::Global);
}
