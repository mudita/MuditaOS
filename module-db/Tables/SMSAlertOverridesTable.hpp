// Copyright (c) 2017-2026, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Common/Common.hpp"
#include "Record.hpp"
#include "Table.hpp"

#include <string>

struct SMSAlertOverridesTableRow : public Record
{
    std::string normalizedNumber;
    std::string soundPath;
    uint32_t ignoreDnd = 0;
    uint32_t forceVibration = 0;
    uint32_t forceMaxVolume = 0;
    uint32_t repeatIntervalSec = 30;
    uint32_t enabled   = 0;
};

enum class SMSAlertOverridesTableFields
{
    normalizedNumber
};

class SMSAlertOverridesTable : public Table<SMSAlertOverridesTableRow, SMSAlertOverridesTableFields>
{
  public:
    explicit SMSAlertOverridesTable(Database *db);
    ~SMSAlertOverridesTable() override = default;

    bool create() override final;
    bool add(SMSAlertOverridesTableRow entry) override final;
    bool removeById(uint32_t id) override final;
    bool update(SMSAlertOverridesTableRow entry) override final;
    SMSAlertOverridesTableRow getById(uint32_t id) override final;
    std::vector<SMSAlertOverridesTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<SMSAlertOverridesTableRow> getLimitOffsetByField(uint32_t offset,
                                                                 uint32_t limit,
                                                                 SMSAlertOverridesTableFields field,
                                                                 const char *str) override final;
    uint32_t count() override final;
    uint32_t countByFieldId(const char *field, uint32_t id) override final;

    SMSAlertOverridesTableRow getByNumber(const std::string &normalizedNumber);
};