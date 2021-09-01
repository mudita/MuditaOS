// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>

#include <module-db/Interface/MultimediaFilesRecord.hpp>

#include <string>

namespace db::multimedia_files::query
{
    class Get : public Query
    {
      public:
        const uint32_t id;
        explicit Get(uint32_t id);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class GetResult : public QueryResult
    {
        const MultimediaFilesRecord record;

      public:
        explicit GetResult(const MultimediaFilesRecord &record);
        [[nodiscard]] auto getResult() const -> MultimediaFilesRecord;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::multimedia_files::query
