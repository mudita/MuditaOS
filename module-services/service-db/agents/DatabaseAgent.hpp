// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Database/Database.hpp>
#include <Service/Service.hpp>

#include <string>

class Database;

class DatabaseAgent
{
  public:
    DatabaseAgent(sys::Service *parentService);

    virtual void initDb()                                          = 0;
    virtual void deinitDb()                                        = 0;
    virtual void registerMessages()                                = 0;
    [[nodiscard]] virtual auto getAgentName() -> const std::string = 0;

  protected:
    [[nodiscard]] virtual auto getDbInitString() -> const std::string = 0;
    [[nodiscard]] virtual auto getDbFilePath() -> const std::string   = 0;

    sys::Service *parentService;
    std::unique_ptr<Database> database;
};
