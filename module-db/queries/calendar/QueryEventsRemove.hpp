#pragma once

#include "module-db/Interface/EventsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::events
{

    class Remove : public Query
    {
      public:
        Remove(const uint32_t &id);
        Remove(const std::string &id);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        uint32_t id;
        std::string UID;
    };

    class RemoveResult : public QueryResult
    {
        const bool ret = true;

      public:
        RemoveResult(const bool &ret);
        [[nodiscard]] auto getResult() const -> bool;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::events
