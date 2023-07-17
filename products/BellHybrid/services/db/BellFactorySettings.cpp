// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <db/BellFactorySettings.hpp>
#include <log/log.hpp>
#include <service-db/SettingsMessages.hpp>
#include <service-db/agents/settings/Settings_queries.hpp>
#include <serial-number-parser/DeviceMetadata.hpp>

namespace settings
{
    auto BellFactorySettings::getMfgEntries() const -> std::unique_ptr<QueryResult>
    {
        const auto [serialNumber, deviceVersionMetadata] = serial_number_parser::getDeviceMetadata();
        const auto [colour, version]                     = deviceVersionMetadata;
        auto factoryData                                 = std::make_unique<QueryResult>();

        factoryData->addRow({Field(factory::serial_number_key), Field(serialNumber.c_str())});
        factoryData->addRow({Field(factory::case_colour_key), Field(colour.c_str())});
        factoryData->addRow({Field(factory::device_version_key), Field(std::to_string(version).c_str())});

        return factoryData;
    }

    auto BellFactorySettings::initDb(Database *database) const -> void
    {
        const auto factoryData = getMfgEntries();

        if (factoryData->getRowCount() <= 0) {
            LOG_FATAL("No eMMC factory data available!");
            return;
        }

        do {
            const auto key        = (*factoryData)[0].getString();
            const auto value      = (*factoryData)[1].getCString();
            const auto path       = settings::factory::entry_key + std::string("/") + key;
            const auto pathString = settings::EntryPath{"", "", "", path, settings::SettingsScope::Global}.to_string();

            if (!database->execute(settings::Statements::insertValue, pathString.c_str(), value)) {
                LOG_ERROR("Failed to set entry '%s' = '%s'!", pathString.c_str(), value);
            }
            else {
                LOG_INFO("Set entry '%s' = '%s'", pathString.c_str(), value);
            }

        } while (factoryData->nextRow());
    }
} // namespace settings
