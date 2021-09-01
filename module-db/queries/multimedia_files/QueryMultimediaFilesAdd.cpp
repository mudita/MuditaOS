// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryMultimediaFilesAdd.hpp"

namespace db::multimedia_files::query
{
    Add::Add(const MultimediaFilesRecord &record) : Query(Query::Type::Create), record(record)
    {}

    auto Add::getRecord() const -> MultimediaFilesRecord
    {
        return record;
    }

    auto Add::debugInfo() const -> std::string
    {
        return std::string{"Add"};
    }

    AddResult::AddResult(bool ret) : ret(ret)
    {}

    auto AddResult::getResult() const -> bool
    {
        return ret;
    }

    auto AddResult::debugInfo() const -> std::string
    {
        return std::string{"AddResult"};
    }
} // namespace db::multimedia_files::query
