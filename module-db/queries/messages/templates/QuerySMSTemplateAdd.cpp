// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "QuerySMSTemplateAdd.hpp"
#include "SMSTemplateRecord.hpp"

#include <string>

using namespace db::query;

SMSTemplateAdd::SMSTemplateAdd(const SMSTemplateRecord &rec) : Query(Query::Type::Create), rec(rec)
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
