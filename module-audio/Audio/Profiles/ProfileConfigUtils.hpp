// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <filesystem>
#include <Audio/codec.hpp>

namespace audio
{
    [[nodiscard]] const audio::codec::Configuration loadConfigurationFromFile(std::filesystem::path filePath);
    const std::string readFile(std::filesystem::path filePath);

} // namespace audio
