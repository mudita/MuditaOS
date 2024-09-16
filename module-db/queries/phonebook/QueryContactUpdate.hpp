// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Common/Query.hpp"
#include <queries/RecordQuery.hpp>
#include <queries/Filter.hpp>
#include <Interface/ContactRecord.hpp>

#include <string>

namespace db::query
{

    class ContactUpdate : public Query
    {
      public:
        ContactUpdate(const ContactRecord &rec);
        ContactRecord rec;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class ContactUpdateResult : public QueryResult
    {
      public:
        ContactUpdateResult(bool result, std::uint32_t id, const std::vector<utils::PhoneNumber::View> &duplicates);
        auto getResult() -> bool
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
        [[nodiscard]] auto getID() const noexcept -> std::uint32_t
        {
            return id;
        }
        [[nodiscard]] auto debugInfo() const -> std::string override;

      private:
        bool result;
        std::vector<utils::PhoneNumber::View> duplicates;
        std::uint32_t id;
    };

}; // namespace db::query
