// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Database/Database.hpp>
#include <Tables/MultimediaFilesTable.hpp>

namespace db::multimedia_files
{
    class MultimediaFilesDB : public Database
    {
      public:
        explicit MultimediaFilesDB(const char *name);

        MultimediaFilesTable files;
    };
} // namespace db::multimedia_files
