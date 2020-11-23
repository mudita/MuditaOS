// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "sqlite3.h"

#include "QueryResult.hpp"

#include <vfs.hpp>
#include <memory>
#include <set>

class DatabaseInitializer;

class Database
{
    static constexpr char INIT_SCRIPTS_EXT[] = "sql";

  public:
    Database(const char *name);

    virtual ~Database();

    std::unique_ptr<QueryResult> query(const char *format, ...);

    bool execute(const char *format, ...);

    // Must be invoked prior creating any database object in order to initialize database OS layer
    static void initialize();
    // Must be invoked before closing system in order to properly close OS layer
    static void deinitialize();

    bool isInitialized() const
    {
        return isInitialized_;
    }

    bool storeIntoFile(const std::string &backupPath);

    uint32_t getLastInsertRowId();
    void pragmaQuery(const std::string &pragmaStatemnt);

    [[nodiscard]] std::string getName() const
    {
        return dbName;
    }

  private:
    const uint32_t maxQueryLen = (8 * 1024);

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
    bool isInitialized_;
    std::unique_ptr<DatabaseInitializer> initializer;
};
