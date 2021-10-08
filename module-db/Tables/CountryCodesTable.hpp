// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Common/Common.hpp"
#include "Database/Database.hpp"
#include "Record.hpp"
#include "Table.hpp"
#include "utf8/UTF8.hpp"
#include <string>

struct CodesTableRow : public Record
{
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
    bool create() override final;
    uint32_t count() override final;
    bool add(CodesTableRow entry) override final
    {
        return (true);
    }
    bool removeById(uint32_t id) override final
    {
        return (true);
    }
    bool update(CodesTableRow entry) override final
    {
        return (true);
    }
    CodesTableRow getById(uint32_t id) override final
    {
        return CodesTableRow();
    }
    std::vector<CodesTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final
    {
        return std::vector<CodesTableRow>();
    }
    std::vector<CodesTableRow> getLimitOffsetByField(uint32_t offset,
                                                     uint32_t limit,
                                                     CodesTableFields field,
                                                     const char *str) override final
    {
        return std::vector<CodesTableRow>();
    }
    uint32_t countByFieldId(const char *field, uint32_t id) override final
    {
        return count();
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
