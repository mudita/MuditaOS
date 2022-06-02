// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSTemplateAdd.hpp"
#include "SMSTemplateRecord.hpp"

#include <string>

using namespace db::query;

SMSTemplateAdd::SMSTemplateAdd(const SMSTemplateRecord &rec) : Query(Query::Type::Read), rec(rec)
{}

SMSTemplateAddResult::SMSTemplateAddResult(bool result, unsigned int id) : result(result), id(id)
{}

[[nodiscard]] auto SMSTemplateAdd::debugInfo() const -> std::string
{
    return "SMSTemplateAdd";
}

[[nodiscard]] auto SMSTemplateAddResult::debugInfo() const -> std::string
{
    return "SMSTemplateAddResult";
}
