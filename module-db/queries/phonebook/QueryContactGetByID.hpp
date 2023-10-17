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

    class ContactGetByID : public Query
    {
      public:
        ContactGetByID(unsigned int id, bool withTemporary = false);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        auto getID() -> unsigned int
        {
            return id;
        }
        auto getWithTemporary() -> bool
        {
            return withTemporary;
        }

      private:
        unsigned int id;
        bool withTemporary = false;
    };

    class ContactGetByIDResult : public QueryResult
    {
      public:
        ContactGetByIDResult(const ContactRecord &record);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        auto getResult() -> ContactRecord
        {
            return std::move(record);
        }

      private:
        ContactRecord record;
    };

}; // namespace db::query
