// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Interface/NotificationsRecord.hpp>
#include <Common/Query.hpp>
#include <string>

namespace db::query::notifications
{
    class GetAll : public Query
    {
      public:
        GetAll();

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class GetAllResult : public QueryResult
    {
        std::unique_ptr<std::vector<NotificationsRecord>> records;

      public:
        explicit GetAllResult(std::unique_ptr<std::vector<NotificationsRecord>> records);
        [[nodiscard]] auto getResult() -> std::unique_ptr<std::vector<NotificationsRecord>>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::notifications
