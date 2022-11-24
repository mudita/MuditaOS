// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#if DEBUG_DB_MODEL_DATA == 1
#define debug_db_data(...) LOG_DEBUG(__VA_ARGS__)
#define error_db_data(...) LOG_ERROR(__VA_ARGS__)
#else
#define debug_db_data(...)
#define error_db_data(...)
#endif
