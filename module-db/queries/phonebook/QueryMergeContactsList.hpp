// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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
        MergeContactsListResult(const std::vector<std::pair<db::Query::Type, uint32_t>> &addedContacts);
        std::vector<std::pair<db::Query::Type, uint32_t>> &getResult();
        [[nodiscard]] auto debugInfo() const -> std::string override;

      private:
        std::vector<std::pair<db::Query::Type, uint32_t>> addedContacts{};
    };

}; // namespace db::query
