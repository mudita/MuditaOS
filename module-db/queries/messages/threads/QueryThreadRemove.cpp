// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryThreadRemove.hpp"

using namespace std::string_literals;

namespace db::query
{
    ThreadRemove::ThreadRemove(unsigned int _id) : Query{Query::Type::Delete}, id{_id}
    {}

    std::string ThreadRemove::debugInfo() const
    {
        return "ThreadRemove"s;
    }

    ThreadRemoveResult::ThreadRemoveResult(bool _result) : result{_result}
    {}

    bool ThreadRemoveResult::success() const noexcept
    {
        return result;
    }

    std::string ThreadRemoveResult::debugInfo() const
    {
        return "ThreadRemoveResult"s;
    }
} // namespace db::query
