// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryMultimediaFilesEdit.hpp"

namespace db::multimedia_files::query
{
    Edit::Edit(const MultimediaFilesRecord &record) : Query(Query::Type::Update), record(record)
    {}

    auto Edit::getRecord() const -> MultimediaFilesRecord
    {
        return record;
    }

    auto Edit::debugInfo() const -> std::string
    {
        return std::string{"Edit"};
    }

    EditResult::EditResult(bool ret) : ret(ret)
    {}

    auto EditResult::getResult() const -> bool
    {
        return ret;
    }

    auto EditResult::debugInfo() const -> std::string
    {
        return std::string{"EditResult"};
    }
} // namespace db::multimedia_files::query
