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
        ThreadGetByNumber(utils::PhoneNumber::View number);
        auto getNumber() const -> const utils::PhoneNumber::View &;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class ThreadGetByNumberResult : public QueryResult
    {
        ThreadRecord thread;

      public:
        ThreadGetByNumberResult(ThreadRecord record);
        [[nodiscard]] auto getThread() -> ThreadRecord;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

}; // namespace db::query
