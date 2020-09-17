#pragma once

#include "Table.hpp"
#include "Record.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

struct SettingsTableRow_v2 : public Record
{
    std::string path;
    std::string value;
};

enum class SettingsTableFields_v2
{
    Path,
    Value
};

class SettingsTable_v2 : public Table<SettingsTableRow_v2, SettingsTableFields_v2>
{
  public:
    SettingsTable_v2(Database *db);
    virtual ~SettingsTable_v2() = default;
    bool create() override final;
    bool add(SettingsTableRow_v2 entry) override final;
    bool removeById(uint32_t id) override final;
    bool removeByField(SettingsTableFields_v2 field, const char *str) override final;
    bool update(SettingsTableRow_v2 entry) override final;
    SettingsTableRow_v2 getById(uint32_t id) override final;
    SettingsTableRow_v2 getByPath(const std::string &path);
    uint32_t count() override final;
    uint32_t countByFieldId(const char *field, uint32_t id) override final;
    std::vector<SettingsTableRow_v2> getLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<SettingsTableRow_v2> getLimitOffsetByField(uint32_t offset,
                                                           uint32_t limit,
                                                           SettingsTableFields_v2 field,
                                                           const char *str) override final;

  private:
    template <typename T> SettingsTableRow_v2 getBy(const char *statement, T arg) const;

    enum
    {
        id,
        path,
        value
    };
};
