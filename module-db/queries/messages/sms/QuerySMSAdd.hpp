// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

#include <Common/Query.hpp>
#include <module-db/Interface/SMSRecord.hpp>

namespace db::query
{
    class SMSAdd : public Query
    {
      public:
        explicit SMSAdd(const SMSRecord &_record);

        [[nodiscard]] std::string debugInfo() const override;
        SMSRecord record;
    };

    class SMSAddResult : public QueryResult
    {
      public:
        explicit SMSAddResult(const SMSRecord &_record, bool _result);

        [[nodiscard]] std::string debugInfo() const override;

        [[nodiscard]] bool succeed() const noexcept;

        SMSRecord record;
        bool result;
    };
} // namespace db::query
