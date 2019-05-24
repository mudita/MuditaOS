
/*
 * @file Database.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "Database.hpp"
#include <assert.h>
#include <memory>
#include "log/log.hpp"


Database::Database(const char *name) :
        dbConnection(nullptr) {

    auto rc = sqlite3_open(name, &dbConnection);
    assert(rc == SQLITE_OK);

}


Database::~Database() {
    sqlite3_close(dbConnection);
}

bool Database::Execute(const char *format, ...) {

    if (!format) { return false; }

    va_list ap;
    auto szQuery = std::make_unique<char[]>(maxQueryLen);
    va_start(ap, format);
    int res = vsnprintf(szQuery.get(), maxQueryLen, format, ap);
    va_end(ap);

    if (res == -1) {
        LOG_ERROR("SQL Query truncated (and not execute) for format: %s", format);
        return nullptr;
    }


    int result = sqlite3_exec(dbConnection, szQuery.get(), NULL, NULL, NULL);

    return result != SQLITE_OK ? false : true;
}

std::unique_ptr<QueryResult> Database::Query(const char *format, ...) {

    if (!format) { return nullptr; }

    va_list ap;
    auto szQuery = std::make_unique<char[]>(maxQueryLen);
    va_start(ap, format);
    int res = vsnprintf(szQuery.get(), maxQueryLen, format, ap);
    va_end(ap);

    if (res == -1) {
        LOG_ERROR("SQL Query truncated (and not execute) for format: %s", format);
        return nullptr;
    }


    auto queryResult = std::make_unique<QueryResult>();

    int result = sqlite3_exec(dbConnection, szQuery.get(), queryCallback, queryResult.get(), NULL);
    if (result != SQLITE_OK) {
        LOG_ERROR("SQL query failed selecting : %d", result);
        return nullptr;
    }

    return queryResult;
}

int Database::queryCallback(void *usrPtr, int count, char **data, char **columns) {
    QueryResult *db = reinterpret_cast<QueryResult *>(usrPtr);

    std::vector<Field> row;
    for (uint32_t i = 0; i < count; i++) {
        row.push_back(Field{data[i]});
    }

    db->AddRow(row);

    return 0;
}
