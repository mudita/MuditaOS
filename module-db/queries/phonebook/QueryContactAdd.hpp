// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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
        ContactAdd(const ContactRecord &rec);
        ContactRecord rec;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class ContactAddResult : public QueryResult
    {
      public:
        ContactAddResult(bool result, unsigned int id);
        [[nodiscard]] auto getResult() const noexcept -> bool
        {
            return result;
        }
        [[nodiscard]] auto getID() const noexcept -> unsigned int
        {
            return id;
        }
        [[nodiscard]] auto debugInfo() const -> std::string override;

      private:
        bool result;
        unsigned int id;
    };

}; // namespace db::query
