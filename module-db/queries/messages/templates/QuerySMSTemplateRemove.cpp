// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSTemplateRemove.hpp"

namespace db::query
{
    SMSTemplateRemove::SMSTemplateRemove(unsigned int id) : Query(Query::Type::Read), id(id)
    {}

    auto SMSTemplateRemove::debugInfo() const -> std::string
    {
        return "SMSTemplateRemove";
    }

    SMSTemplateRemoveResult::SMSTemplateRemoveResult(bool result) : result(result)
    {}
    auto SMSTemplateRemoveResult::getResults() const -> bool
    {
        return result;
    }
    auto SMSTemplateRemoveResult::debugInfo() const -> std::string
    {
        return "SMSTemplateRemoveResult";
    }

} // namespace db::query
