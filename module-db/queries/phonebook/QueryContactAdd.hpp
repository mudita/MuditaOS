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

    class ContactAdd : public Query
    {
      public:
        explicit ContactAdd(const ContactRecord &rec);
        ContactRecord rec;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class ContactAddResult : public QueryResult
    {
      public:
        ContactAddResult(bool result, unsigned int id, const std::vector<utils::PhoneNumber::View> &duplicates);
        [[nodiscard]] auto getResult() const noexcept -> bool
        {
            return result;
        }
        [[nodiscard]] auto hasDuplicates() const noexcept -> bool
        {
            return !duplicates.empty();
        }
        [[nodiscard]] auto getDuplicates() const noexcept -> std::vector<utils::PhoneNumber::View>
        {
            return duplicates;
        }
        [[nodiscard]] auto getID() const noexcept -> unsigned int
        {
            return id;
        }
        [[nodiscard]] auto debugInfo() const -> std::string override;

      private:
        bool result;
        std::vector<utils::PhoneNumber::View> duplicates;
        unsigned int id;
    };

}; // namespace db::query
