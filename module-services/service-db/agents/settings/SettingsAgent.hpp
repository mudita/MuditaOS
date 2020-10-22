// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "messages/SettingsMessages.hpp"
#include "agents/DatabaseAgent.hpp"

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
    auto getValue(Settings::EntryPath path) -> std::optional<std::string>;
    auto setValue(Settings::EntryPath path, std::string value) -> std::string;
    auto getDbInitString() -> const std::string override;
    auto getDbFilePath() -> const std::string override;
    auto handleGetVariable(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    auto handleSetVariable(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
};
