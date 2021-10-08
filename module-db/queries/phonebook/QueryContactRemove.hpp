// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common/Query.hpp"
#include <queries/RecordQuery.hpp>
#include <queries/Filter.hpp>
#include <Interface/ContactRecord.hpp>

#include <string>

namespace db::query
{

    class ContactRemove : public Query
    {
      public:
        ContactRemove(unsigned int id);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        auto getID() -> unsigned int
        {
            return id;
        }

      private:
        unsigned int id;
    };

    class ContactRemoveResult : public QueryResult
    {
      public:
        ContactRemoveResult(bool result);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        auto getResult() -> bool
        {
            return result;
        }

      private:
        bool result;
    };

}; // namespace db::query
