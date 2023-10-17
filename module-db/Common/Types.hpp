// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

/// 32-bit unsigned integer
#define u32_ "%" PRIu32
/// 32-bit unsigned integer with comma, for instance: 323,
#define u32_c "%" PRIu32 ","
/// Zero terminated string with single-quotes on both ends, for instance: 'my string'
#define str_ "%Q"
/// The same as above with additional comma at the end, for instance: 'my string',
#define str_c "%Q,"