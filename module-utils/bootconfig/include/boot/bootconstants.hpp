// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
namespace boot::consts
{
    inline constexpr auto tar_buf       = 8192 * 4;
    inline constexpr auto crc_char_size = 9;
    inline constexpr auto crc_radix     = 16;
    inline constexpr auto ext_crc32     = ".crc32";
    inline constexpr auto crc_buf       = 1024;
} // namespace boot::consts
