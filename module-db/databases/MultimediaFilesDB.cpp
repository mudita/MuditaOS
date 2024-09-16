// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "MultimediaFilesDB.hpp"

namespace db::multimedia_files
{
    MultimediaFilesDB::MultimediaFilesDB(const char *name) : Database(name), files(this)
    {}
} // namespace db::multimedia_files
