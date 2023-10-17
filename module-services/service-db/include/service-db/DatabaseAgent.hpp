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
    explicit DatabaseAgent(sys::Service *parentService);
    virtual ~DatabaseAgent() = default;

    virtual void registerMessages()                                = 0;
    virtual void unRegisterMessages()                              = 0;
    [[nodiscard]] virtual auto getAgentName() -> const std::string = 0;

    static constexpr auto ZERO_ROWS_FOUND = 0;
    static constexpr auto ONE_ROW_FOUND   = 1;

  protected:
    sys::Service *parentService;
    std::unique_ptr<Database> database;
};
