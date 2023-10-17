// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <db/PureFactorySettings.hpp>

#include <log/log.hpp>
#include <service-db/SettingsMessages.hpp>
#include <service-db/agents/settings/Settings_queries.hpp>

namespace settings
{
    PureFactorySettings::PureFactorySettings(const std::string &filePath) : filePath(filePath)
    {}

    auto PureFactorySettings::readMfgSettings() const -> json11::Json
    {
        std::ifstream file(filePath.c_str());
        const std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        std::string parserError;
        auto factoryObj = json11::Json::parse(content, parserError);

        if (!parserError.empty()) {
            LOG_FATAL("Factory data parse error: %s", parserError.c_str());
        }

        return factoryObj;
    }

    auto PureFactorySettings::getMfgEntries() const -> std::unique_ptr<QueryResult>
    {
        auto factoryData          = std::make_unique<QueryResult>();
        const auto factoryContent = readMfgSettings();

        for (const auto &[path, value] : factoryContent.object_items()) {
            factoryData->addRow({Field(path.c_str()), Field(value.string_value().c_str())});
        }

        return factoryData;
    }

    auto PureFactorySettings::initDb(Database *database) const -> void
    {
        const auto factoryData = getMfgEntries();

        if (factoryData->getRowCount() <= 0) {
            LOG_FATAL("No EEPROM factory data available!");
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
