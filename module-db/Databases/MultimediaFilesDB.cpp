// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MultimediaFilesDB.hpp"

namespace db::multimedia_files
{
    MultimediaFilesDB::MultimediaFilesDB(const char *name) : Database(name), files(this)
    {}
} // namespace db::multimedia_files
