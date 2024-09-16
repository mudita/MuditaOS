// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once
#include <filesystem>
#include <Audio/codec.hpp>

namespace audio
{
    [[nodiscard]] const audio::codec::Configuration loadConfigurationFromFile(std::filesystem::path filePath);
    const std::string readFile(std::filesystem::path filePath);

} // namespace audio
