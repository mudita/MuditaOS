// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common/Query.hpp"
#include <queries/RecordQuery.hpp>
#include <Interface/ContactRecord.hpp>

#include <cstdint>
#include <string>

namespace db::query
{
    class ContactGetByNumberID : public Query
    {
      public:
        explicit ContactGetByNumberID(std::uint32_t numberID);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const std::uint32_t numberID;
    };

    class ContactGetByNumberIDResult : public QueryResult
    {
      public:
        explicit ContactGetByNumberIDResult(const ContactRecord &record);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        auto getResult() const -> ContactRecord
        {
            return record;
        }

      private:
        ContactRecord record;
    };

} // namespace db::query
