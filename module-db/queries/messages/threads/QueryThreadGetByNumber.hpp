// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>
#include <Interface/ThreadRecord.hpp>
#include <PhoneNumber.hpp>

#include <string>

namespace db::query
{

    class ThreadGetByNumber : public Query
    {
        utils::PhoneNumber::View number;

      public:
        explicit ThreadGetByNumber(const utils::PhoneNumber::View &number);
        [[nodiscard]] auto getNumber() const -> const utils::PhoneNumber::View &;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class ThreadGetByNumberResult : public QueryResult
    {
        ThreadRecord thread;

      public:
        explicit ThreadGetByNumberResult(ThreadRecord record);
        [[nodiscard]] auto getThread() -> ThreadRecord;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

}; // namespace db::query
