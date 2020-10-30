// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <optional> // for optional
#include <string>   // for string

#include "messages/SettingsMessages.hpp"
#include "agents/DatabaseAgent.hpp" // for DatabaseAgent
#include "Service/Message.hpp"      // for DataMessage (ptr only), Message_t, ResponseMessage (ptr only)
#include <map>

namespace Settings
{
    struct EntryPath;
} // namespace Settings
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

  private:
    using MapOfRecipentsToBeNotified = std::map<std::string, std::set<std::string>>;
    MapOfRecipentsToBeNotified variableChangeRecipents;
    using SetOfRecipents = std::set<std::string>;
    SetOfRecipents profileChangedRecipents;
    SetOfRecipents modeChangeRecipents;
    // db operations
    auto dbGetValue(Settings::EntryPath path) -> std::optional<std::string>;
    auto dbSetValue(Settings::EntryPath path, std::string value) -> bool;
    auto dbRegisterValueChange(Settings::EntryPath path) -> bool;
    auto dbUnregisterValueChange(Settings::EntryPath path) -> bool;

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
    auto getDbFilePath() -> const std::string override;

    // msg handlers
    // variable
    auto handleGetVariable(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleSetVariable(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleRegisterOnVariableChange(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleUnregisterOnVariableChange(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;

    // profile
    auto handleRegisterProfileChange(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleUnregisterProfileChange(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleSetCurrentProfile(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleGetCurrentProfile(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleAddProfile(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleListProfiles(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;

    // mode
    auto handleRegisterOnModeChange(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleUnregisterOnModeChange(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleSetCurrentMode(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleGetCurrentMode(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleAddMode(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleListModes(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
};
