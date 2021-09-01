// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Interface/MultimediaFilesRecord.hpp>

#include <Common/Query.hpp>

#include <string>

namespace db::multimedia_files::query
{
    class Add : public Query
    {
        const MultimediaFilesRecord record;

      public:
        explicit Add(const MultimediaFilesRecord &record);
        [[nodiscard]] auto getRecord() const -> MultimediaFilesRecord;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class AddResult : public QueryResult
    {
        const bool ret = false;

      public:
        explicit AddResult(bool ret);
        [[nodiscard]] auto getResult() const -> bool;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::multimedia_files::query
