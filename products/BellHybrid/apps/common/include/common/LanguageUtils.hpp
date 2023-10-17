// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

namespace utils::language
{
    auto getCorrectMinutesNumeralForm(std::uint32_t val) -> std::string;
    auto getCorrectSecondsNumeralForm(std::uint32_t val) -> std::string;

    auto getCorrectMinutesAccusativeForm(std::uint32_t val) -> std::string;
} // namespace utils::language
