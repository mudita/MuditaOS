// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "sqlite3.h"
#include "QueryResult.hpp"

#include <memory>
#include <stdexcept>
#include <filesystem>

class DatabaseInitializer;

class DatabaseInitialisationError : public std::runtime_error
{
  public:
    using std::runtime_error::runtime_error;
};

class Database
{
  public:
    explicit Database(const char *name, bool readOnly = false);
    virtual ~Database();

    std::unique_ptr<QueryResult> query(const char *format, ...);

    bool execute(const char *format, ...);

    // Must be invoked prior creating any database object in order to initialize database OS layer
    static bool initialize();

    // Must be invoked before closing system in order to properly close OS layer
    static bool deinitialize();

    bool storeIntoFile(const std::filesystem::path &backupPath);

    uint32_t getLastInsertRowId();
    void pragmaQuery(const std::string &pragmaStatement);

    auto pragmaQueryForValue(const std::string &pragmaStatement, const std::int32_t value) -> bool;

    [[nodiscard]] bool isInitialized() const noexcept
    {
        return isInitialized_;
    }

    [[nodiscard]] std::string getName() const
    {
        return dbName;
    }

  private:
    static constexpr auto InitScriptExtension  = "sql";
    static constexpr std::uint32_t maxQueryLen = (8 * 1024);

    void initQueryStatementBuffer();
    void clearQueryStatementBuffer();

    void populateDbAppId();

    /*
     * Arguments:
     *
     *   usrPtr - Pointer to user data
     *    count - The number of columns in the result set
     *     data - The row's data
     *  columns - The column names
     */
    static int queryCallback(void *usrPtr, int count, char **data, char **columns);

  protected:
    sqlite3 *dbConnection;
    std::string dbName;
    char *queryStatementBuffer;
    bool isInitialized_;
    std::unique_ptr<DatabaseInitializer> initializer;
};
