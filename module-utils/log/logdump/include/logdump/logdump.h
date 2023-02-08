// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    /// @return: < 0 - error occurred during log flush
    /// @return:   0 - log flush did not happen
    /// @return:   1 - log flush successful
    int diagnosticDumpLogs();
    int shutdownFlushLogs();
    int loggerFlush();

#ifdef __cplusplus
}
#endif
