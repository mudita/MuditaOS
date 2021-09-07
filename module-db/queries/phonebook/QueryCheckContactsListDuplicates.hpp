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

    class CheckContactsListDuplicates : public Query
    {
      public:
        CheckContactsListDuplicates(std::vector<ContactRecord> contacts);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        std::vector<ContactRecord> &getContactsList();

      private:
        std::vector<ContactRecord> contacts;
    };

    class CheckContactsListDuplicatesResult : public QueryResult
    {
      public:
        explicit CheckContactsListDuplicatesResult(
            std::pair<std::vector<ContactRecord>, std::vector<ContactRecord>> result);
        std::vector<ContactRecord> &getUnique();
        std::vector<ContactRecord> &getDuplicates();

        [[nodiscard]] auto debugInfo() const -> std::string override;

      private:
        std::vector<ContactRecord> unique;
        std::vector<ContactRecord> duplicates;
    };

}; // namespace db::query
