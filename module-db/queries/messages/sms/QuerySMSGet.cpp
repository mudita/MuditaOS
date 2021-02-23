// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSGet.hpp"

#include <string>

using namespace db::query;

SMSGet::SMSGet(std::size_t limit, std::size_t offset) : RecordQuery(limit, offset)
{}

SMSGetResult::SMSGetResult(const std::vector<SMSRecord> &records) : RecordQueryResult(records)
{}

[[nodiscard]] auto SMSGet::debugInfo() const -> std::string
{
    return "SMSGet";
}

[[nodiscard]] auto SMSGetResult::debugInfo() const -> std::string
{
    return "SMSGetResult";
}
