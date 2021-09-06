// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryMultimediaFilesRemove.hpp"

namespace db::multimedia_files::query
{
    Remove::Remove(const uint32_t id) : Query(Query::Type::Delete), id(id)
    {}

    auto Remove::debugInfo() const -> std::string
    {
        return std::string{"Remove"};
    }

    RemoveResult::RemoveResult(const bool ret) : ret(ret)
    {}

    auto RemoveResult::getResult() const -> bool
    {
        return ret;
    }

    auto RemoveResult::debugInfo() const -> std::string
    {
        return std::string{"RemoveResult"};
    }

    RemoveAll::RemoveAll() : Query(Query::Type::Delete)
    {}

    auto RemoveAll::debugInfo() const -> std::string
    {
        return std::string{"RemoveAll"};
    }

    RemoveByPath::RemoveByPath(const std::string &path) : Query(Query::Type::Delete), path(path)
    {}

    auto RemoveByPath::debugInfo() const -> std::string
    {
        return std::string{"RemoveByPath"};
    }

} // namespace db::multimedia_files::query
