// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsAgent.hpp"
#include "Settings_queries.hpp"

#include <Database/Database.hpp>
#include <Service/Service.hpp>
#include <purefs/filesystem_paths.hpp>
#include <service-db/SettingsCache.hpp>

namespace settings
{
    namespace factory
    {
        constexpr auto data_file         = "personalization.json";
        const std::filesystem::path path = purefs::dir::getMfgConfPath() / data_file;
    } // namespace factory

} // namespace settings

SettingsAgent::SettingsAgent(sys::Service *parentService, const std::string dbName, settings::SettingsCache *cache)
    : DatabaseAgent(parentService), cache(cache), factorySettings(settings::factory::path), dbName{dbName}
{
    if (nullptr == cache) {
        this->cache = settings::SettingsCache::getInstance();
    }

    database = std::make_unique<Database>(getDbFilePath().c_str());
}

void SettingsAgent::initDb()
{
    factorySettings.initDb(database.get());

    // first approach -> take care about big amount of variables
    auto allVars = database->query(settings::Statements::getAllValues);
    if (nullptr == allVars || 0 == allVars->getRowCount()) {
        return;
    }

    do {
        auto path  = (*allVars)[0].getString();
        auto value = (*allVars)[1].getString();
        settings::EntryPath variablePath;
        variablePath.parse(path);
        cache->setValue(variablePath, value);
    } while (allVars->nextRow());
}

void SettingsAgent::deinitDb()
{
    database->deinitialize();
}

void SettingsAgent::registerMessages()
{
    // connect handler & message in parent service
    using std::placeholders::_1;

    // single variable
    parentService->connect(settings::Messages::GetVariable(), std::bind(&SettingsAgent::handleGetVariable, this, _1));
    parentService->connect(settings::Messages::SetVariable(), std::bind(&SettingsAgent::handleSetVariable, this, _1));
    parentService->connect(settings::Messages::RegisterOnVariableChange(),
                           std::bind(&SettingsAgent::handleRegisterOnVariableChange, this, _1));
    parentService->connect(settings::Messages::UnregisterOnVariableChange(),
                           std::bind(&SettingsAgent::handleUnregisterOnVariableChange, this, _1));
}

auto SettingsAgent::getDbInitString() -> const std::string
{
    return {};
}

auto SettingsAgent::getDbFilePath() -> const std::string
{
    return (purefs::dir::getUserDiskPath() / dbName).string();
}
auto SettingsAgent::getAgentName() -> const std::string
{
    return std::string("settingsAgent");
}

// dbSingleVar
auto SettingsAgent::dbGetValue(const settings::EntryPath &path) -> std::optional<std::string>
{
    auto retQuery = database->query(settings::Statements::getValue, path.to_string().c_str());
    if (nullptr == retQuery || 1 != retQuery->getRowCount()) {
        return std::string{};
    }
    return (*retQuery)[0].getString();
}

auto SettingsAgent::dbSetValue(const settings::EntryPath &path, const std::string &value) -> bool
{
    /// insert or update
    return database->execute(settings::Statements::insertValue, path.to_string().c_str(), value.c_str());
}

auto SettingsAgent::dbRegisterValueChange(const settings::EntryPath &path) -> bool
{
    return database->execute(settings::Statements::setNotification, path.to_string().c_str(), path.service.c_str());
}

auto SettingsAgent::dbUnregisterValueChange(const settings::EntryPath &path) -> bool
{
    return database->execute(
        settings::Statements::clearNotificationdRow, path.to_string().c_str(), path.service.c_str());
}

auto SettingsAgent::handleGetVariable(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::GetVariable *>(req)) {
        auto path  = msg->getPath();
        auto value = dbGetValue(path);
        return std::make_shared<settings::Messages::VariableResponse>(std::move(path), std::move(value));
    }
    return std::make_shared<sys::ResponseMessage>();
}

auto SettingsAgent::handleSetVariable(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::SetVariable *>(req)) {

        auto path     = msg->getPath();
        auto value    = msg->getValue().value_or("");
        auto oldValue = dbGetValue(path);
        if (oldValue.has_value() && oldValue.value() != value) {
            dbSetValue(path, value);
            for (const auto &regPath : variableChangeRecipients[path.to_string()]) {
                if (regPath.service != path.service) {
                    auto updateMsg =
                        std::make_shared<settings::Messages::VariableChanged>(regPath, value, oldValue.value_or(""));
                    parentService->bus.sendUnicast(std::move(updateMsg), regPath.service);
                    LOG_DEBUG("[SettingsAgent::handleSetVariable] notified service: %s", regPath.service.c_str());
                }
            }
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}

auto SettingsAgent::handleRegisterOnVariableChange(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::RegisterOnVariableChange *>(req)) {
        auto path = msg->getPath();
        if (dbRegisterValueChange(path)) {
            if (auto it = variableChangeRecipients.find(path.to_string()); it == variableChangeRecipients.end()) {
                variableChangeRecipients[path.to_string()] = {path};
            }
            else if (it->second.find(path) == it->second.end()) {
                it->second.insert(path);
            }
            else {
                return std::make_shared<sys::ResponseMessage>();
            }
            auto currentValue = dbGetValue(path).value_or("");
            LOG_DEBUG("[SettingsAgent::handleRegisterOnVariableChange] %s", path.to_string().c_str());
            auto msgValue =
                std::make_shared<::settings::Messages::VariableChanged>(std::move(path), std::move(currentValue), "");
            parentService->bus.sendUnicast(std::move(msgValue), msg->sender);
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}

auto SettingsAgent::handleUnregisterOnVariableChange(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::UnregisterOnVariableChange *>(req); msg != nullptr) {
        auto path = msg->getPath();
        if (dbUnregisterValueChange(path)) {
            if (auto it = variableChangeRecipients.find(path.to_string()); it != variableChangeRecipients.end()) {
                LOG_DEBUG("[SettingsAgent::handleUnregisterOnVariableChange] %s", path.to_string().c_str());
                it->second.erase(path);
            }
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}
