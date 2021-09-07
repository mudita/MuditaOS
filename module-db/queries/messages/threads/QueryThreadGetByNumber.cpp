// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryThreadGetByNumber.hpp"

#include <Interface/ThreadRecord.hpp>
#include <PhoneNumber.hpp>

#include <string>

namespace db::query
{
    ThreadGetByNumber::ThreadGetByNumber(const utils::PhoneNumber::View &number)
        : Query(Query::Type::Read), number(number)
    {}

    auto ThreadGetByNumber::getNumber() const -> const utils::PhoneNumber::View &
    {
        return number;
    }

    [[nodiscard]] auto ThreadGetByNumber::debugInfo() const -> std::string
    {
        return "ThreadGetByNumber";
    }

    ThreadGetByNumberResult::ThreadGetByNumberResult(ThreadRecord thread) : thread(std::move(thread))
    {}

    auto ThreadGetByNumberResult::getThread() -> ThreadRecord
    {
        return std::move(thread);
    }

    [[nodiscard]] auto ThreadGetByNumberResult::debugInfo() const -> std::string
    {
        return "ThreadGetByNumberResult";
    }
} // namespace db::query
