// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "module-db/Database/Database.hpp"

#include <log.hpp>

#include <purefs/filesystem_paths.hpp>
#include <gsl/util>

#include <cassert>
#include <cstring>
#include <memory>

#include <service-db/agents/settings/Settings_queries.hpp>

#include <map>
#include <string>

class DatabaseInitializer
{
  public:
    DatabaseInitializer(Database *)
    {}
    virtual ~DatabaseInitializer() = default;
};

std::map<std::string, std::string> variables;
std::string profile = "_startProfileValue";
std::string mode    = "_initialModeValue";
std::set<std::string> profiles, modes;
const std::string system_phone_mode("system/phone_mode");
const std::string system_phone_profile("system/phone_profile");

bool stubExecute(const std::string &format, const std::string &path, const std::string &val)
{
    if (format.empty()) {
        return false;
    }

    if (format == std::string(settings::Statements::insertValue)) {
        LOG_DEBUG("Database::execute set %s", path.c_str());
        variables[path] = val;
        return true;
    }

    if (format == std::string(settings::Statements::setNotification)) {
        return true;
    }
    if (format == std::string(settings::Statements::clearNotificationdRow)) {
        return true;
    }

    if (format == std::string(settings::Statements::updateValue)) {
        // profile change, mode change
        if (val == system_phone_mode) {
            mode = path;
            return true;
        }
        if (val == system_phone_profile) {
            profile = path;
            return true;
        }
        return false;
    }

    if (format == std::string(settings::Statements::addDictValue)) {
        // add new profile, add new mode
        if (path == system_phone_mode) {
            modes.insert(val);
            return true;
        }
        if (path == system_phone_profile) {
            profiles.insert(val);
            return true;
        }
        return false;
    }

    return false;
}

std::unique_ptr<QueryResult> stubQuery(const std::string &format, const std::string &what)
{
    std::vector<Field> row;
    auto queryResult = std::make_unique<QueryResult>();
    if (std::string(settings::Statements::getValue) == format) {
        if (system_phone_mode == what) // settings::DbPaths::phone_mode
        {
            row.push_back(Field{mode.c_str()});
        }
        else if (system_phone_profile == what) // settings::DbPaths::phone_profile
        {
            row.push_back(Field{profile.c_str()});
        }
        else // variable
        {
            if (variables.end() == variables.find(what)) {
                variables[what] = what + " _initialValue";
            }
            LOG_DEBUG("Database::query for %s", what.c_str());
            row.push_back(Field{variables[what].c_str()});
        }
        queryResult->addRow(row);
    }
    else if (std::string(settings::Statements::getDictValue) == format) {
        if (system_phone_mode == what) {
            for (const auto &mode : modes) {
                row.clear();
                row.push_back(Field{mode.c_str()});
                queryResult->addRow(row);
            }
        }
        else if (system_phone_profile == what) {
            for (const auto &profile : profiles) {
                row.clear();
                row.push_back(Field{profile.c_str()});
                queryResult->addRow(row);
            }
        }
        // allProfiles, allModes
    }
    return queryResult;
}

Database::Database(const char *name, bool)
    : dbName(name), queryStatementBuffer{nullptr}, isInitialized_(false),
      initializer(std::make_unique<DatabaseInitializer>(this))
{
    isInitialized_ = true;
}

void Database::initQueryStatementBuffer()
{}

void Database::clearQueryStatementBuffer()
{
    std::memset(queryStatementBuffer, 0, maxQueryLen);
}

Database::~Database()
{}

bool Database::initialize()
{
    return true;
}

bool Database::deinitialize()
{
    return true;
}

bool Database::execute(const char *format, ...)
{
    if (format == nullptr) {
        return false;
    }
    std::string format_str(format);
    if (format_str.find("%") == std::string::npos) {
        return true;
    }
    std::string path, val;
    va_list ap;
    va_start(ap, format);
    format = va_arg(ap, const char *);
    if (format != nullptr) {
        path   = std::string(format);
        format = va_arg(ap, const char *);
        if (format != nullptr) {
            val = std::string(format);
        }
    }
    va_end(ap);
    if (!path.empty() && !val.empty()) {
        return stubExecute(format_str, path, val);
    }
    return true;
}

std::unique_ptr<QueryResult> Database::query(const char *format, ...)
{
    if (format == nullptr) {
        return nullptr;
    }

    std::string format_str(format);
    if (format_str.find("%") == std::string::npos) {
        return nullptr;
    }
    std::string what;
    va_list ap;
    va_start(ap, format);
    format = va_arg(ap, const char *);
    if (format != nullptr) {
        what = std::string(format);
    }
    va_end(ap);
    if (what.empty()) {
        return nullptr;
    }
    return stubQuery(format_str, what);
}

int Database::queryCallback(void *usrPtr, int count, char **data, char **columns)
{
    return 0;
}

uint32_t Database::getLastInsertRowId()
{
    return 1;
}

void Database::pragmaQuery(const std::string &pragmaStatemnt)
{}

bool Database::storeIntoFile(const std::filesystem::path &backupPath)
{
    return true;
}
