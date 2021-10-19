// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSAdd.hpp"

using namespace std::literals::string_literals;

namespace db::query
{
    SMSAdd::SMSAdd(const SMSRecord &_record) : Query(Query::Type::Create), record{_record}
    {}

    std::string SMSAdd::debugInfo() const
    {
        return "SMSAdd"s;
    }

    SMSAddResult::SMSAddResult(const SMSRecord &_record, bool _result) : record{_record}, result{_result}
    {}

    std::string SMSAddResult::debugInfo() const
    {
        return "SMSAddResult"s;
    }

    bool SMSAddResult::succeed() const noexcept
    {
        return result;
    }
} // namespace db::query
