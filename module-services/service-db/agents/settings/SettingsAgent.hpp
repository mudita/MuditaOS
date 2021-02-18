// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/DatabaseAgent.hpp>
#include <service-db/SettingsMessages.hpp>
#include <Service/Message.hpp>

#include <map>
#include <optional>
#include <string>

namespace settings
{
    struct EntryPath;
} // namespace settings
namespace sys
{
    class Service;
} // namespace sys

class SettingsAgent : public DatabaseAgent
{
  public:
    SettingsAgent(sys::Service *parentService);
    ~SettingsAgent() = default;

    void initDb() override;
    void deinitDb() override;
    void registerMessages() override;
    auto getAgentName() -> const std::string override;
    auto getDbFilePath() -> const std::string override;

  private:
    // using MapOfRecipentsToBeNotified = std::map<std::string, std::set<std::string>>;
    using MapOfRecipentsToBeNotified = std::map<std::string, std::set<settings::EntryPath>>;
    MapOfRecipentsToBeNotified variableChangeRecipents;
    using SetOfRecipents = std::set<std::string>;
    SetOfRecipents profileChangedRecipents;
    SetOfRecipents modeChangeRecipents;
    // db operations
    auto dbGetValue(settings::EntryPath path) -> std::optional<std::string>;
    auto dbSetValue(settings::EntryPath path, std::string value) -> bool;
    auto dbRegisterValueChange(settings::EntryPath path) -> bool;
    auto dbUnregisterValueChange(settings::EntryPath path) -> bool;

    auto dbRegisterOnProfileChange(const std::string &service) -> bool;
    auto dbUnregisterOnProfileChange(const std::string &service) -> bool;
    auto dbSetCurrentProfile(const std::string &profile) -> bool;
    auto dbGetCurrentProfile() -> std::string;
    auto dbGetAllProfiles() -> std::list<std::string>;
    auto dbAddProfile(const std::string &profile) -> bool;

    auto dbRegisterOnModeChange(const std::string &service) -> bool;
    auto dbUnregisterOnModeChange(const std::string &service) -> bool;
    auto dbSetCurrentMode(const std::string &mode) -> bool;
    auto dbGetCurrentMode() -> std::string;
    auto dbGetAllModes() -> std::list<std::string>;
    auto dbAddMode(const std::string &mode) -> bool;

    auto getDbInitString() -> const std::string override;

    // msg handlers
    // variable
    auto handleGetVariable(sys::Message *req) -> sys::MessagePointer;
    auto handleSetVariable(sys::Message *req) -> sys::MessagePointer;
    auto handleRegisterOnVariableChange(sys::Message *req) -> sys::MessagePointer;
    auto handleUnregisterOnVariableChange(sys::Message *req) -> sys::MessagePointer;

    // profile
    auto handleRegisterProfileChange(sys::Message *req) -> sys::MessagePointer;
    auto handleUnregisterProfileChange(sys::Message *req) -> sys::MessagePointer;
    auto handleSetCurrentProfile(sys::Message *req) -> sys::MessagePointer;
    auto handleGetCurrentProfile(sys::Message *req) -> sys::MessagePointer;
    auto handleAddProfile(sys::Message *req) -> sys::MessagePointer;
    auto handleListProfiles(sys::Message *req) -> sys::MessagePointer;

    // mode
    auto handleRegisterOnModeChange(sys::Message *req) -> sys::MessagePointer;
    auto handleUnregisterOnModeChange(sys::Message *req) -> sys::MessagePointer;
    auto handleSetCurrentMode(sys::Message *req) -> sys::MessagePointer;
    auto handleGetCurrentMode(sys::Message *req) -> sys::MessagePointer;
    auto handleAddMode(sys::Message *req) -> sys::MessagePointer;
    auto handleListModes(sys::Message *req) -> sys::MessagePointer;
};
