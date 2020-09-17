#include "SettingsTable_v2.hpp"
#include <cassert>

namespace statements
{
    constexpr auto createTableQuery = " CREATE TABLE IF NOT EXISTS settings_v2("
                                      " _id INTEGER PRIMARY KEY, "
                                      " path TEXT NOT NULL UNIQUE, "
                                      " value TEXT); ";

    constexpr auto addItem = "INSERT or IGNORE INTO settings_v2(path, value)"
                             "VALUES('%q', '%q');";

    constexpr auto deleteById    = "DELETE FROM settings_v2 where _id = %lu;";
    constexpr auto deleteByField = "DELETE FROM settings_v2 WHERE %q = '%q'";

    constexpr auto updateItem = "UPDATE settings_v2 SET path = '%q', value = '%q' WHERE _id = %lu;";

    constexpr auto getById           = "SELECT * FROM settings_v2 WHERE _id= %u;";
    constexpr auto getByPath         = "SELECT * FROM settings_v2 WHERE path = '%q'";
    constexpr auto getCount          = "SELECT COUNT(*) FROM settings_v2;";
    constexpr auto getAllLimitOffset = "SELECT * from settings_v2 LIMIT %lu OFFSET %lu;";

} // namespace statements

SettingsTable_v2::SettingsTable_v2(Database *db) : Table(db)
{}

bool SettingsTable_v2::create()
{
    if (!db->execute(statements::createTableQuery)) {
        return false;
    }

    return true;
}

bool SettingsTable_v2::add(SettingsTableRow_v2 entry)
{
    if (!entry.path.empty() && !entry.value.empty()) {
        return db->execute(statements::addItem, entry.path.c_str(), entry.value.c_str());
    }
    return false;
}

bool SettingsTable_v2::removeById(uint32_t id)
{
    return db->execute(statements::deleteById, id);
}

bool SettingsTable_v2::removeByField(SettingsTableFields_v2 field, const char *str)
{
    std::string fieldName = [&field]() -> std::string {
        switch (field) {
        case SettingsTableFields_v2::Path:
            return "path";
        case SettingsTableFields_v2::Value:
            return "value";
        default:
            return "";
        }
    }();
    return db->execute(statements::deleteByField, fieldName.c_str(), str);
}

bool SettingsTable_v2::update(SettingsTableRow_v2 entry)
{
    return db->execute(statements::updateItem, entry.path.c_str(), entry.value.c_str(), entry.ID);
}

SettingsTableRow_v2 SettingsTable_v2::getById(uint32_t id)
{
    return getBy(statements::getById, id);
}

SettingsTableRow_v2 SettingsTable_v2::getByPath(const std::string &path)
{
    return getBy(statements::getByPath, path.c_str());
}

template <typename T> SettingsTableRow_v2 SettingsTable_v2::getBy(const char *statement, T arg) const
{
    auto retQuery = db->query(statement, arg);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return SettingsTableRow_v2();
    }

    return SettingsTableRow_v2{
        (*retQuery)[id].getUInt32(),
        (*retQuery)[path].getString(),
        (*retQuery)[value].getString(),
    };
}

uint32_t SettingsTable_v2::count()
{
    auto queryRet = db->query(statements::getCount);

    if (queryRet == nullptr || queryRet->getRowCount() == 0) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}

uint32_t SettingsTable_v2::countByFieldId(const char *field, uint32_t id)
{
    assert(0 && "Not implemented");

    return 0;
}

std::vector<SettingsTableRow_v2> SettingsTable_v2::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query(statements::getAllLimitOffset, limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<SettingsTableRow_v2>();
    }

    std::vector<SettingsTableRow_v2> ret;

    do {
        ret.push_back(SettingsTableRow_v2{
            (*retQuery)[id].getUInt32(),
            (*retQuery)[path].getString(),
            (*retQuery)[value].getString(),
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<SettingsTableRow_v2> SettingsTable_v2::getLimitOffsetByField(uint32_t offset,
                                                                         uint32_t limit,
                                                                         SettingsTableFields_v2 field,
                                                                         const char *str)
{
    assert(0 && "Not implemented");

    return std::vector<SettingsTableRow_v2>();
}
