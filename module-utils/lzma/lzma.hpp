// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <filesystem>

namespace utils::lzma
{
    void decompress(const std::filesystem::path &src, const std::filesystem::path &dst);
}
