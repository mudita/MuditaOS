// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Database/Database.hpp>
#include <Service/Service.hpp>

#include <string>
#include <memory>

class Database;
namespace sys
{
    class Service;
} // namespace sys

class DatabaseAgent
{
  public:
    DatabaseAgent(sys::Service *parentService);
    virtual ~DatabaseAgent() = default;

    virtual void initDb()                                          = 0;
    virtual void deinitDb()                                        = 0;
    virtual void registerMessages()                                = 0;
    [[nodiscard]] virtual auto getAgentName() -> const std::string = 0;
    bool storeIntoFile(const std::filesystem::path &file)
    {
        if (database != nullptr)
            return database->storeIntoFile(file);

        return false;
    }
    [[nodiscard]] virtual auto getDbFilePath() -> const std::string = 0;

    static constexpr auto ZERO_ROWS_FOUND = 0;
    static constexpr auto ONE_ROW_FOUND   = 1;

  protected:
    [[nodiscard]] virtual auto getDbInitString() -> const std::string = 0;

    sys::Service *parentService;
    std::unique_ptr<Database> database;
};
