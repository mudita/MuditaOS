// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
