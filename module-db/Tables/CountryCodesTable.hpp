#pragma once
#include "Common/Common.hpp"
#include "Database/Database.hpp"
#include "Table.hpp"
#include "utf8/UTF8.hpp"
#include <string>

struct CodesTableRow
{
    uint32_t id;
    uint32_t mcc;
    uint32_t mnc;
    std::string iso;
    std::string country;
    uint32_t country_code;
    std::string network;
};

enum CodesTableFields
{
};

class CountryCodesTable : public Table<CodesTableRow, CodesTableFields>
{
  public:
    CountryCodesTable(Database *db);
    virtual ~CountryCodesTable();
    bool Create() override final;
    uint32_t GetCount() override final;
    bool Add(CodesTableRow entry) override final
    {
        return (true);
    }
    bool RemoveByID(uint32_t id) override final
    {
        return (true);
    }
    bool Update(CodesTableRow entry) override final
    {
        return (true);
    }
    CodesTableRow GetByID(uint32_t id) override final
    {
        return CodesTableRow();
    }
    std::vector<CodesTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final
    {
        return std::vector<CodesTableRow>();
    }
    std::vector<CodesTableRow> GetLimitOffsetByField(uint32_t offset,
                                                     uint32_t limit,
                                                     CodesTableFields field,
                                                     const char *str) override final
    {
        return std::vector<CodesTableRow>();
    }
    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final
    {
        return GetCount();
    }
    CodesTableRow GetByMCC(uint32_t mcc);

  private:
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS codes("
                                   "_id INTEGER PRIMARY KEY,"
                                   "mcc INTEGER,"
                                   "mnc INTEGER,"
                                   "iso TEXT NOT NULL,"
                                   "country TEXT NOT NULL,"
                                   "country_code INTEGER,"
                                   "network TEXT NOT NULL);";
};
