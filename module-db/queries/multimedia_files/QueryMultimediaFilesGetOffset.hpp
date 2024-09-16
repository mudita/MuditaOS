// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "module-db/Interface/MultimediaFilesRecord.hpp"

#include <Common/Query.hpp>

namespace db::multimedia_files::query
{
    class GetOffsetByPath : public Query
    {
      public:
        GetOffsetByPath(const std::string &folderPath,
                        const std::string &recordPath,
                        SortingBy sorting = SortingBy::TitleAscending);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const std::string folderPath;
        const std::string recordPath;
        const SortingBy sorting = SortingBy::TitleAscending;
    };

    class GetOffsetResult : public QueryResult
    {
      public:
        explicit GetOffsetResult(SortedRecord sortedRecord);
        [[nodiscard]] auto getResult() const -> SortedRecord;
        [[nodiscard]] auto debugInfo() const -> std::string override;

      private:
        const SortedRecord sortedRecord;
    };
} // namespace db::multimedia_files::query
