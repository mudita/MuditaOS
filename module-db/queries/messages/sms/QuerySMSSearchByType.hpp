// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>
#include <Common/Common.hpp>
#include <module-db/Interface/SMSRecord.hpp>

namespace db::query
{
    /// implements search for SMS by type
    class SMSSearchByType : public Query
    {
      public:
        SMSType type;
        unsigned int starting_postion;
        unsigned int depth = 0; // 0 meaning no depth limit
        SMSSearchByType(SMSType type_to_search, unsigned int starting_position, unsigned int depth = 0);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSSearchByTypeResult : public QueryResult
    {
        std::vector<SMSRecord> results;
        unsigned int results_max_depth = 0;

      public:
        SMSSearchByTypeResult(unsigned int results_max_depth, std::vector<SMSRecord> result_rows);
        [[nodiscard]] auto getResults() const -> std::vector<SMSRecord>;
        [[nodiscard]] auto getMaxDepth() const -> unsigned int;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
