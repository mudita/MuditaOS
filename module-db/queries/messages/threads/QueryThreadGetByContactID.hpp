#pragma once

#include <string>
#include <optional>

#include <Common/Query.hpp>
#include <Interface/ThreadRecord.hpp>

namespace db::query
{
    class ThreadGetByContactID : public Query
    {
      public:
        explicit ThreadGetByContactID(unsigned int _id);

        [[nodiscard]] std::string debugInfo() const override;

        unsigned int id;
    };

    class ThreadGetByContactIDResult : public QueryResult
    {
      public:
        explicit ThreadGetByContactIDResult(const std::optional<ThreadRecord> &_record);

        std::optional<ThreadRecord> getRecord() const noexcept;
        [[nodiscard]] std::string debugInfo() const override;

      private:
        std::optional<ThreadRecord> record;
    };
} // namespace db::query
