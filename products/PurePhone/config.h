// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#if defined(TARGET_Linux) && not defined(SERIAL_PORT)
#define ENABLE_GSM (0)
#else
#define ENABLE_GSM (1)
#endif
