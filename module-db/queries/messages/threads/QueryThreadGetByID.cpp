// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryThreadGetByID.hpp"

using namespace std::string_literals;

namespace db::query
{
    ThreadGetByID::ThreadGetByID(unsigned int _id) : Query{Query::Type::Read}, id{_id}
    {}

    std::string ThreadGetByID::debugInfo() const
    {
        return "ThreadGetByID"s;
    }

    ThreadGetByIDResult::ThreadGetByIDResult(const std::optional<ThreadRecord> &_record) : record{_record}
    {}

    std::optional<ThreadRecord> ThreadGetByIDResult::getRecord() const noexcept
    {
        return record;
    }

    std::string ThreadGetByIDResult::debugInfo() const
    {
        return "ThreadGetByIDResult"s;
    }
} // namespace db::query
