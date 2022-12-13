// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FactorySettings.hpp"

#include <service-db/DatabaseAgent.hpp>
#include <service-db/SettingsMessages.hpp>
#include <Service/Message.hpp>

#include <map>
#include <optional>
#include <string>

namespace settings
{
    struct EntryPath;
    class SettingsCache;
} // namespace settings
namespace sys
{
    class Service;
} // namespace sys

class SettingsAgent : public DatabaseAgent
{
  public:
    SettingsAgent(sys::Service *parentService, std::string dbName, settings::SettingsCache *cache = nullptr);
    ~SettingsAgent() = default;

    void registerMessages() override;
    void unRegisterMessages() override;
    auto getAgentName() -> const std::string override;

  private:
    settings::SettingsCache *cache;
    settings::FactorySettings factorySettings;

    using MapOfRecipentsToBeNotified = std::map<std::string, std::set<settings::EntryPath>>;
    MapOfRecipentsToBeNotified variableChangeRecipients;
    using SetOfRecipents = std::set<std::string>;
    SetOfRecipents profileChangedRecipients;
    SetOfRecipents modeChangeRecipients;
    const std::string dbName;

    // db operations
    auto dbGetValue(const settings::EntryPath &path) -> std::optional<std::string>;
    auto dbSetValue(const settings::EntryPath &path, const std::string &value) -> bool;
    auto dbRegisterValueChange(const settings::EntryPath &path) -> bool;
    auto dbUnregisterValueChange(const settings::EntryPath &path) -> bool;

    // msg handlers
    // variable
    auto handleGetVariable(sys::Message *req) -> sys::MessagePointer;
    auto handleSetVariable(sys::Message *req) -> sys::MessagePointer;
    auto handleRegisterOnVariableChange(sys::Message *req) -> sys::MessagePointer;
    auto handleUnregisterOnVariableChange(sys::Message *req) -> sys::MessagePointer;
};
