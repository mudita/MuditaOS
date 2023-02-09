// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <functional>

template <typename T>
struct SettingsParam
{
    T initValue;
    std::function<void(T newValue)> onChangeCallback{nullptr};
};

struct MeditationParams
{
    SettingsParam<std::int32_t> meditationDuration;
    SettingsParam<std::chrono::minutes> intervalChime;
};
