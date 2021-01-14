// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsAgent.hpp"
#include "Settings_queries.hpp"

#include <Database/Database.hpp>
#include <Service/Service.hpp>
#include <purefs/filesystem_paths.hpp>

namespace settings::DbPaths
{
    constexpr auto phone_mode    = "system/phone_mode";
    constexpr auto phone_profile = "system/phone_profile";
}; // namespace settings::DbPaths

SettingsAgent::SettingsAgent(sys::Service *parentService) : DatabaseAgent(parentService)
{
    database = std::make_unique<Database>(getDbFilePath().c_str());
}

void SettingsAgent::initDb()
{
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
auto SettingsAgent::dbGetValue(settings::EntryPath path) -> std::optional<std::string>
{
    auto retQuery = database->query(settings::Statements::getValue, path.variable.c_str());
    if (nullptr == retQuery || 1 != retQuery->getRowCount()) {
        return std::string{};
    }
    return (*retQuery)[0].getString();
}

auto SettingsAgent::dbSetValue(settings::EntryPath path, std::string value) -> bool
{
    /// insert or update
    return database->execute(settings::Statements::insertValue, path.to_string().c_str(), value.c_str());
}

auto SettingsAgent::dbRegisterValueChange(settings::EntryPath path) -> bool
{
    return database->execute(settings::Statements::setNotification, path.to_string().c_str(), path.service.c_str());
}

auto SettingsAgent::dbUnregisterValueChange(settings::EntryPath path) -> bool
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

        return std::make_shared<settings::Messages::VariableResponse>(path, value);
    }
    return std::make_shared<sys::ResponseMessage>();
};

auto SettingsAgent::handleSetVariable(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::SetVariable *>(req)) {

        auto path     = msg->getPath();
        auto value    = msg->getValue().value_or("");
        auto oldValue = dbGetValue(path);
        if (oldValue.has_value() && oldValue.value() != value) {
            dbSetValue(path, value);
            for (auto regPath : variableChangeRecipents[path.to_string()]) {
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
};

auto SettingsAgent::handleRegisterOnVariableChange(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::RegisterOnVariableChange *>(req)) {
        auto path = msg->getPath();
        if (dbRegisterValueChange(path)) {
            auto it = variableChangeRecipents.find(path.variable);
            if (variableChangeRecipents.end() == it || it->second.end() == it->second.find(path)) {
                variableChangeRecipents[path.to_string()] = {path};
                auto currentValue                         = dbGetValue(path).value_or("");
                auto msgValue = std::make_shared<::settings::Messages::VariableChanged>(path, currentValue, "");
                parentService->bus.sendUnicast(std::move(msgValue), msg->sender);
                LOG_DEBUG("[SettingsAgent::handleRegisterOnVariableChange] %s=%s to %s",
                          path.to_string().c_str(),
                          currentValue.c_str(),
                          msg->sender.c_str());
            }
            else {
                it->second.insert(path);
            }
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}

auto SettingsAgent::handleUnregisterOnVariableChange(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::UnregisterOnVariableChange *>(req)) {
        auto path = msg->getPath();
        if (dbUnregisterValueChange(path)) {
            auto it = variableChangeRecipents.find(path.variable);
            if (variableChangeRecipents.end() != it) {
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
            profileChangedRecipents.insert(msg->sender);
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
            profileChangedRecipents.erase(msg->sender);
        }
    }
    return std::make_shared<sys::ResponseMessage>();
}
auto SettingsAgent::handleSetCurrentProfile(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::SetCurrentProfile *>(req)) {
        auto profile = msg->getProfileName();
        if (dbSetCurrentProfile(profile)) {
            for (auto service : profileChangedRecipents) {
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
        auto service = profileChangedRecipents.find(req->sender);
        if (profileChangedRecipents.end() != service) {
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
            for (auto service : profileChangedRecipents) {
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
        profileChangedRecipents.insert(req->sender);
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
            modeChangeRecipents.insert(msg->sender);
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
        modeChangeRecipents.erase(req->sender);
    }
    return std::make_shared<sys::ResponseMessage>();
}
auto SettingsAgent::handleSetCurrentMode(sys::Message *req) -> sys::MessagePointer
{
    if (auto msg = dynamic_cast<settings::Messages::SetCurrentMode *>(req)) {
        auto newMode = msg->getProfileName();
        if (dbGetCurrentMode() != newMode) {
            if (dbSetCurrentMode(newMode)) {
                for (auto service : modeChangeRecipents) {
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
        if (modeChangeRecipents.end() != modeChangeRecipents.find(req->sender)) {
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
            for (auto service : modeChangeRecipents) {
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
        modeChangeRecipents.insert(req->sender);
        auto msgAllModes = std::make_shared<settings::Messages::ModeListResponse>(dbGetAllModes());
        parentService->bus.sendUnicast(std::move(msgAllModes), req->sender);
    }
    return std::make_shared<sys::ResponseMessage>();
}
