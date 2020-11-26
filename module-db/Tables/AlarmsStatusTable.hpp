// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Common/Common.hpp"
#include "Database/Database.hpp"
#include "Record.hpp"
#include "Table.hpp"
#include <string>

struct AlarmsStatusRow : public Record
{
    std::string name;
};

enum class AlarmsStatusFields
{
    name
};

class AlarmsStatusTable : public Table<AlarmsStatusRow, AlarmsStatusFields>
{
  public:
    explicit AlarmsStatusTable(Database *db);
    ~AlarmsStatusTable() override = default;
    bool create() final;

    uint32_t count() final;
    bool add(AlarmsStatusRow entry) final;
    bool removeById(uint32_t id) final;
    bool update(AlarmsStatusRow entry) final;
    AlarmsStatusRow getById(uint32_t id) final;
    std::vector<AlarmsStatusRow> getLimitOffset(uint32_t offset, uint32_t limit) final;
    std::vector<AlarmsStatusRow> getLimitOffsetByField(uint32_t offset,
                                                       uint32_t limit,
                                                       AlarmsStatusFields field,
                                                       const char *str) final;
    uint32_t countByFieldId(const char *field, uint32_t id) final;
};
