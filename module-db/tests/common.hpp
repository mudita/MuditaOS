// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Database/Database.hpp"
#include <catch2/catch.hpp>
#include <string>
#include <filesystem>

void RemoveDbFiles(const std::string &dbName);

template <typename Db>
class DatabaseUnderTest
{
  public:
    explicit DatabaseUnderTest(std::filesystem::path name)
    {
        Database::initialize();

        if (std::filesystem::exists(name)) {
            std::filesystem::remove(name);
        }

        db = std::make_unique<Db>(name.c_str());

        if (not db->isInitialized()) {
            throw std::runtime_error("Could not initialize database");
        }
    }

    ~DatabaseUnderTest()
    {
        Database::deinitialize();
    }

    Db &get()
    {
        return *db;
    }

  private:
    std::filesystem::path name;
    std::unique_ptr<Db> db;
};
