// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <logdump/logdump.h>
#include <purefs/filesystem_paths.hpp>
#include <Logger.hpp>

int dumpLogs()
{
    return Log::Logger::get().dumpToFile(purefs::dir::getLogsPath() / LOG_FILE_NAME);
}
