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
    namespace DbPaths
    {
        constexpr auto phone_mode    = "system/phone_mode";
        constexpr auto phone_profile = "system/phone_profile";
    } // namespace DbPaths

    namespace factory
    {
        constexpr auto data_file         = "personalization.json";
        const std::filesystem::path path = purefs::dir::getMfgConfPath() / data_file;
    } // namespace factory

} // namespace settings

SettingsAgent::SettingsAgent(sys::Service *parentService, settings::SettingsCache *cache)
    : DatabaseAgent(parentService), cache(cache), factorySettings(settings::factory::path)
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
    // profile
    parentService->connect(settings::Messages::RegisterOnProfileChange(),
                           std::bind(&SettingsAgent::handleRegisterProfileChange, this, _1));
    parentService->connect(settings::Messages::UnregisterOnProfileChange(),
                           std::bind(&SettingsAgent::handleUnregisterProfileChange, this, _1));
    parentService->connect(settings::Messages::SetCurrentProfile(),
                           std::bind(&SettingsAgent::handleSetCurrentProfile, this, _1));
    parentService->connect(settings::Messages::GetCurrentProfile(),
                           std::bind(&SettingsAgent::handleGetCurrentProfile, this, _1));
    parentService->connect(settings::Messages::AddProfile(), std::bind(&SettingsAgent::handleAddProfile, this, _1));
    parentService->connect(settings::Messages::ListProfiles(), std::bind(&SettingsAgent::handleListProfiles, this, _1));

    // mode
    parentService->connect(settings::Messages::RegisterOnModeChange(),
                           std::bind(&SettingsAgent::handleRegisterOnModeChange, this, _1));
    parentService->connect(settings::Messages::UnregisterOnModeChange(),
                           std::bind(&SettingsAgent::handleUnregisterOnModeChange, this, _1));
    parentService->connect(settings::Messages::SetCurrentMode(),
                           std::bind(&SettingsAgent::handleSetCurrentMode, this, _1));
    parentService->connect(settings::Messages::GetCurrentMode(),
                           std::bind(&SettingsAgent::handleGetCurrentMode, this, _1));
    parentService->connect(settings::Messages::AddMode(), std::bind(&SettingsAgent::handleAddMode, this, _1));
    parentService->connect(settings::Messages::ListModes(), std::bind(&SettingsAgent::handleListModes, this, _1));
}

auto SettingsAgent::getDbInitString() -> const std::string
{
    return {};
}

auto SettingsAgent::getDbFilePath() -> const std::string
{
    return (purefs::dir::getUserDiskPath() / "settings_v2.db").string();
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

// db Profile
auto SettingsAgent::dbRegisterOnProfileChange(const std::string &service) -> bool
{
    return database->execute(settings::Statements::setNotification, settings::DbPaths::phone_profile, service.c_str());
}
auto SettingsAgent::dbUnregisterOnProfileChange(const std::string &service) -> bool
{
    return database->execute(
        settings::Statements::clearNotificationdRow, settings::DbPaths::phone_profile, service.c_str());
}
auto SettingsAgent::dbSetCurrentProfile(const std::string &profile) -> bool
{
    return database->execute(settings::Statements::updateValue, profile.c_str(), settings::DbPaths::phone_profile);
}
auto SettingsAgent::dbGetCurrentProfile() -> std::string
{
    auto qProfile = database->query(settings::Statements::getValue, settings::DbPaths::phone_profile);
    if (nullptr == qProfile || 1 != qProfile->getRowCount()) {
        return std::string{};
    }
    return (*qProfile)[0].getString();
}
auto SettingsAgent::dbGetAllProfiles() -> std::list<std::string>
{
    auto qProfiles = database->query(settings::Statements::getDictValue, settings::DbPaths::phone_profile);
    if (nullptr == qProfiles || 0 == qProfiles->getRowCount()) {
        return std::list<std::string>{};
    }
    std::list<std::string> profiles;
    do {
        profiles.push_back((*qProfiles)[0].getString());
    } while (qProfiles->nextRow());
    return profiles;
}
auto SettingsAgent::dbAddProfile(const std::string &profile) -> bool
{
    return database->execute(settings::Statements::addDictValue, settings::DbPaths::phone_profile, profile.c_str());
}

// dbMode
auto SettingsAgent::dbRegisterOnModeChange(const std::string &service) -> bool
{
    return database->execute(settings::Statements::setNotification, settings::DbPaths::phone_mode, service.c_str());
}

auto SettingsAgent::dbUnregisterOnModeChange(const std::string &service) -> bool
{
    return database->execute(
        settings::Statements::clearNotificationdRow, settings::DbPaths::phone_mode, service.c_str());
}

auto SettingsAgent::dbSetCurrentMode(const std::string &mode) -> bool
{
    return database->execute(settings::Statements::updateValue, mode.c_str(), settings::DbPaths::phone_mode);
}

auto SettingsAgent::dbGetCurrentMode() -> std::string
{
    auto qMode = database->query(settings::Statements::getValue, settings::DbPaths::phone_mode);
    if (nullptr == qMode || 1 != qMode->getRowCount()) {
        return std::string{};
    }
    return (*qMode)[0].getString();
}

auto SettingsAgent::dbGetAllModes() -> std::list<std::string>
{
    auto qModes = database->query(settings::Statements::getDictValue, settings::DbPaths::phone_mode);
    if (nullptr == qModes || 0 == qModes->getRowCount()) {
        return std::list<std::string>{};
    }
    std::list<std::string> modes;
    do {
        modes.push_back((*qModes)[0].getString());
    } while (qModes->nextRow());
    return modes;
}

auto SettingsAgent::dbAddMode(const std::string &mode) -> bool
{
    return database->execute(settings::Statements::addDictValue, settings::DbPaths::phone_mode, mode.c_str());
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
            LOG_DEBUG("[SettingsAgent::handleRegisterOnVariableChange] %s=%s to %s",
                      path.to_string().c_str(),
                      currentValue.c_str(),
                      msg->sender.c_str());
            auto msgValue =
                std::make_shared<::settings::Messages::VariableChanged>(std::move(path), std::move(currentValue), "");
            parentService->bus.sendUnicast(std::move(msgValue), msg->sender);
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}

auto SettingsAgent::handleUnregisterOnVariableChange(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::UnregisterOnVariableChange *>(req)) {
        auto path = msg->getPath();
        if (dbUnregisterValueChange(path)) {
            if (auto it = variableChangeRecipients.find(path.to_string()); it != variableChangeRecipients.end()) {
                it->second.erase(path);
                LOG_DEBUG("[SettingsAgent::handleUnregisterOnVariableChange] %s", path.to_string().c_str());
            }
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}

// profile
auto SettingsAgent::handleRegisterProfileChange(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::RegisterOnProfileChange *>(req)) {
        if (dbRegisterOnProfileChange(msg->sender)) {
            profileChangedRecipients.insert(msg->sender);
            auto msgCurrentProfile = std::make_shared<settings::Messages::CurrentProfileChanged>(dbGetCurrentProfile());
            parentService->bus.sendUnicast(std::move(msgCurrentProfile), msg->sender);
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}
auto SettingsAgent::handleUnregisterProfileChange(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::UnregisterOnProfileChange *>(req)) {
        if (dbUnregisterOnProfileChange(msg->sender)) {
            profileChangedRecipients.erase(msg->sender);
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}
auto SettingsAgent::handleSetCurrentProfile(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::SetCurrentProfile *>(req)) {
        auto profile = msg->getProfileName();
        if (dbSetCurrentProfile(profile)) {
            for (const auto &service : profileChangedRecipients) {
                if (service != msg->sender) {
                    auto msgProfileChanged = std::make_shared<settings::Messages::CurrentProfileChanged>(profile);
                    parentService->bus.sendUnicast(std::move(msgProfileChanged), service);
                }
            }
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}
auto SettingsAgent::handleGetCurrentProfile(sys::Message *req) -> sys::MessagePointer
{
    if (nullptr != dynamic_cast<settings::Messages::GetCurrentProfile *>(req)) {
        auto service = profileChangedRecipients.find(req->sender);
        if (profileChangedRecipients.end() != service) {
            auto msgCurrentProfile = std::make_shared<settings::Messages::CurrentProfileChanged>(dbGetCurrentProfile());
            parentService->bus.sendUnicast(std::move(msgCurrentProfile), *service);
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}
auto SettingsAgent::handleAddProfile(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::AddProfile *>(req)) {
        if (dbAddProfile(msg->getProfileName())) {
            auto allProfiles = dbGetAllProfiles();
            for (const auto &service : profileChangedRecipients) {
                if (service != req->sender) {
                    auto msgAllProfiles = std::make_shared<settings::Messages::ProfileListResponse>(allProfiles);
                    parentService->bus.sendUnicast(std::move(msgAllProfiles), service);
                }
            }
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}
auto SettingsAgent::handleListProfiles(sys::Message *req) -> sys::MessagePointer
{
    if (nullptr != dynamic_cast<settings::Messages::ListProfiles *>(req)) {
        profileChangedRecipients.insert(req->sender);
        auto msgAllProfiles = std::make_shared<settings::Messages::ProfileListResponse>(dbGetAllProfiles());
        parentService->bus.sendUnicast(std::move(msgAllProfiles), req->sender);
    }
    return std::make_shared<sys::ResponseMessage>();
}

// mode
auto SettingsAgent::handleRegisterOnModeChange(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::RegisterOnModeChange *>(req)) {
        if (dbRegisterOnModeChange(msg->sender)) {
            modeChangeRecipients.insert(msg->sender);
            auto msgMode = std::make_shared<settings::Messages::CurrentModeChanged>(dbGetCurrentMode());
            parentService->bus.sendUnicast(std::move(msgMode), msg->sender);
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}
auto SettingsAgent::handleUnregisterOnModeChange(sys::Message *req) -> sys::MessagePointer
{
    if (nullptr != dynamic_cast<settings::Messages::UnregisterOnModeChange *>(req)) {
        dbUnregisterOnModeChange(req->sender);
        modeChangeRecipients.erase(req->sender);
    }
    return std::make_shared<sys::ResponseMessage>();
}
auto SettingsAgent::handleSetCurrentMode(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::SetCurrentMode *>(req)) {
        if (auto newMode = msg->getProfileName(); dbGetCurrentMode() != newMode) {
            if (dbSetCurrentMode(newMode)) {
                for (const auto &service : modeChangeRecipients) {
                    if (service != msg->sender) {
                        auto msgModeChanged = std::make_shared<settings::Messages::CurrentModeChanged>(newMode);
                        parentService->bus.sendUnicast(std::move(msgModeChanged), service);
                    }
                }
            }
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}
auto SettingsAgent::handleGetCurrentMode(sys::Message *req) -> sys::MessagePointer
{
    if (nullptr != dynamic_cast<settings::Messages::GetCurrentMode *>(req)) {
        if (modeChangeRecipients.end() != modeChangeRecipients.find(req->sender)) {
            auto msgMode = std::make_shared<settings::Messages::CurrentModeChanged>(dbGetCurrentMode());
            parentService->bus.sendUnicast(std::move(msgMode), req->sender);
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}
auto SettingsAgent::handleAddMode(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::AddMode *>(req)) {
        if (dbAddMode(msg->getProfileName())) {
            auto allModes = dbGetAllModes();
            for (const auto &service : modeChangeRecipients) {
                if (service != msg->sender) {
                    auto msgAllModes = std::make_shared<settings::Messages::ModeListResponse>(allModes);
                    parentService->bus.sendUnicast(std::move(msgAllModes), service);
                }
            }
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}
auto SettingsAgent::handleListModes(sys::Message *req) -> sys::MessagePointer
{
    if (nullptr != dynamic_cast<settings::Messages::ListModes *>(req)) {
        modeChangeRecipients.insert(req->sender);
        auto msgAllModes = std::make_shared<settings::Messages::ModeListResponse>(dbGetAllModes());
        parentService->bus.sendUnicast(std::move(msgAllModes), req->sender);
    }
    return std::make_shared<sys::ResponseMessage>();
}
