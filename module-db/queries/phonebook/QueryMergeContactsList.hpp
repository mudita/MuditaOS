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

    class MergeContactsList : public Query
    {
      public:
        MergeContactsList(std::vector<ContactRecord> contacts);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        std::vector<ContactRecord> &getContactsList();

      private:
        std::vector<ContactRecord> contacts;
    };

    class MergeContactsListResult : public QueryResult
    {
      public:
        MergeContactsListResult(bool result);
        [[nodiscard]] auto getResult() const noexcept -> bool
        {
            return result;
        }
        [[nodiscard]] auto debugInfo() const -> std::string override;

      private:
        bool result = false;
    };

}; // namespace db::query
