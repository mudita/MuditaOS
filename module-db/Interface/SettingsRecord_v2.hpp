#pragma once

#include "Databases/SettingsDB.hpp"
#include "Record.hpp"

#include <memory>
#include <sstream>
#include <string>

// fw declarations
namespace db::query::settings
{
    class SettingsQuery;
    class SettingsResult;
    class AddOrIgnoreQuery;
    class AddOrIgnoreResult;
    class UpdateQuery;
    class UpdateResult;
} // namespace db::query::settings

class SettingsRecord_v2 : public Record
{
  public:
    SettingsRecord_v2()          = default;
    virtual ~SettingsRecord_v2() = default;
    SettingsRecord_v2(const SettingsTableRow_v2 &tableRow);

    friend std::ostream &operator<<(std::ostream &out, const SettingsRecord_v2 &rec);

    [[nodiscard]] auto getPath() const noexcept -> std::string
    {
        return path;
    }

    template <typename T>[[nodiscard]] auto getValue(const T &defaultValue = {}) const -> T
    {
        if (value.empty()) {
            return defaultValue;
        }
        T ret;
        std::istringstream(value) >> ret;
        return ret;
    }

  private:
    std::string path{};
    std::string value{};
};

enum class SettingsRecordField_v2
{
    GroupID,
    Path,
    Value
};

class SettingsRecordInterface_v2 : public RecordInterface<SettingsRecord_v2, SettingsRecordField_v2>
{
  public:
    SettingsRecordInterface_v2(SettingsDB *settingsDB);
    virtual ~SettingsRecordInterface_v2() override = default;

    SettingsRecord_v2 GetByPath(const std::string &path);
    SettingsRecord_v2 GetByID(uint32_t id) override final;
    bool Add(const SettingsRecord_v2 &entry) override final;
    uint32_t GetCount() override final;
    bool RemoveByID(uint32_t id) override final;
    bool Update(const SettingsRecord_v2 &recUpdated) override final;
    bool Update(const std::string &path, const std::string &value) noexcept;
    std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

  private:
    SettingsDB *settingsDB = nullptr;

    std::unique_ptr<db::query::settings::SettingsResult> runQueryImpl(const db::query::settings::SettingsQuery *query);
};
