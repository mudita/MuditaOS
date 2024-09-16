// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "QueryMultimediaFilesGetOffset.hpp"

namespace db::multimedia_files::query
{
    GetOffsetByPath::GetOffsetByPath(const std::string &folderPath, const std::string &recordPath, SortingBy sorting)
        : Query(Query::Type::Read), folderPath{folderPath}, recordPath{recordPath}, sorting{sorting}
    {}

    [[nodiscard]] auto GetOffsetByPath::debugInfo() const -> std::string
    {
        return std::string{"GetOffsetByPath"};
    }

    GetOffsetResult::GetOffsetResult(SortedRecord sortedRecord) : sortedRecord(std::move(sortedRecord))
    {}

    [[nodiscard]] auto GetOffsetResult::getResult() const -> SortedRecord
    {
        return sortedRecord;
    }

    [[nodiscard]] auto GetOffsetResult::debugInfo() const -> std::string
    {
        return std::string{"GetOffsetResult"};
    }
} // namespace db::multimedia_files::query
