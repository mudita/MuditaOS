// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FactorySettings.hpp"

#include <log/log.hpp>
#include <service-db/SettingsMessages.hpp>

namespace settings
{

    std::unique_ptr<QueryResult> FactorySettings::getMfgEntries()
    {
        auto factoryContent                      = readMfgSettings(filePath);
        std::unique_ptr<QueryResult> factoryData = std::make_unique<QueryResult>();

        for (const auto &[path, value] : factoryContent.object_items()) {
            factoryData->addRow({Field(path.c_str()), Field(value.string_value().c_str())});
        }

        return factoryData;
    }

    void FactorySettings::initDb(Database *database)
    {
        auto factoryData = getMfgEntries();
        if (factoryData->getRowCount()) {
            do {

                settings::EntryPath variablePath{"",
                                                 "",
                                                 "",
                                                 settings::factory::entry_key + std::string("/") +
                                                     (*factoryData)[0].getString(),
                                                 settings::SettingsScope::Global};
                auto value = (*factoryData)[1].getString();

                if (!database->execute(
                        settings::Statements::insertValue, variablePath.to_string().c_str(), value.c_str())) {
                    break;
                }

                LOG_INFO("Put factory entry: %s=%s", variablePath.to_string().c_str(), value.c_str());

            } while (factoryData->nextRow());
        }
    }
    json11::Json FactorySettings::readMfgSettings(const std::string &path)
    {
        std::ifstream file(path.c_str());
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        std::string parserError;
        auto factoryObj = json11::Json::parse(content, parserError);

        if (!parserError.empty()) {
            LOG_FATAL("Factory data parse error: %s", parserError.c_str());
        }

        return factoryObj;
    }
} // namespace settings
