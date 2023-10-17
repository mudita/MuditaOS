// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryMultimediaFilesGet.hpp"

namespace db::multimedia_files::query
{
    Get::Get(uint32_t id) : Query(Query::Type::Read), id(id)
    {}

    auto Get::debugInfo() const -> std::string
    {
        return std::string{"Get"};
    }

    GetByPath::GetByPath(const std::string &path) : Query(Query::Type::Read), path(path)
    {}

    auto GetByPath::debugInfo() const -> std::string
    {
        return std::string{"GetByPath"};
    }

    GetResult::GetResult(const MultimediaFilesRecord &record) : record(record)
    {}

    auto GetResult::getResult() const -> MultimediaFilesRecord
    {
        return record;
    }

    auto GetResult::debugInfo() const -> std::string
    {
        return std::string{"GetResult"};
    }
} // namespace db::multimedia_files::query
