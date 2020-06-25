#pragma once

#include "Interface/NotificationsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::calllog
{
    class SetAllRead : public Query
    {
      public:
        SetAllRead();

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SetAllReadResult : public QueryResult
    {
      public:
        SetAllReadResult(bool ret);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const bool ret = true;
    };

}; // namespace db::query::calllog
