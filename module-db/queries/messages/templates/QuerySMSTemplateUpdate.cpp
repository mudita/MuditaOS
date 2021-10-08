// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSTemplateUpdate.hpp"
#include "SMSTemplateRecord.hpp"

#include <string>

using namespace db::query;

SMSTemplateUpdate::SMSTemplateUpdate(const SMSTemplateRecord &rec) : Query(Query::Type::Read), rec(std::move(rec))
{}

SMSTemplateUpdateResult::SMSTemplateUpdateResult(bool result) : result(result)
{}

[[nodiscard]] auto SMSTemplateUpdate::debugInfo() const -> std::string
{
    return "SMSTemplateUpdate";
}

[[nodiscard]] auto SMSTemplateUpdateResult::debugInfo() const -> std::string
{
    return "SMSTemplateUpdateResult";
}
