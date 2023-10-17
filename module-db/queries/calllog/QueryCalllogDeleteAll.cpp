// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryCalllogDeleteAll.hpp"

namespace db::query::calllog
{
    DeleteAll::DeleteAll() : Query(Query::Type::Update)
    {}

    auto DeleteAll::debugInfo() const -> std::string
    {
        return "QueryDeleteAll";
    }

    DeleteAllResult::DeleteAllResult(bool ret) : ret(ret)
    {}

    [[nodiscard]] auto DeleteAllResult::debugInfo() const -> std::string
    {
        return "DeleteAllResult";
    }
} // namespace db::query::calllog
