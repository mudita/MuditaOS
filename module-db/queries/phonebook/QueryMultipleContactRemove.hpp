// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common/Query.hpp"
#include <queries/RecordQuery.hpp>
#include <queries/Filter.hpp>
#include <Interface/ContactRecord.hpp>

#include <string>
#include <vector>

namespace db::query
{

    class MultipleContactRemove : public Query
    {
      public:
        MultipleContactRemove(const std::vector<std::uint32_t> &ids);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        auto getIDs() -> std::vector<std::uint32_t>
        {
            return ids;
        }

      private:
        std::vector<std::uint32_t> ids;
    };

    class MultipleContactRemoveResult : public QueryResult
    {
      public:
        MultipleContactRemoveResult(bool result);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        auto getResult() -> bool
        {
            return result;
        }

      private:
        bool result;
    };

}; // namespace db::query
