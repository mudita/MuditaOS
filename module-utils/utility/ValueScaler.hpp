// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <algorithm>
#include <optional>
#include <cmath>
#include <array>

namespace utils
{
    template <typename T>
    struct Range
    {
        T min;
        T max;
    };

    /// Perform scaling of the input value based on the input&output ranges. If the input values is not contained within
    /// the provided input range return std::nullopt.
    template <typename T>
    std::optional<T> scale_value(const Range<T> inputRange, const Range<T> outputRange, const T input)
    {
        if (input > inputRange.max || input < inputRange.min) {
            return {};
        }

        const auto inRangeVal  = inputRange.max - inputRange.min;
        const auto outRangeVal = outputRange.max - outputRange.min;

        if (outRangeVal == 0 || inRangeVal == 0) {
            return outputRange.min;
        }
        float slope = 1.0 * (outRangeVal) / (inRangeVal);
        auto output = outputRange.min + slope * (input - inputRange.min);

        return static_cast<T>(std::floor(output));
    }

    template <typename T>
    struct Entry
    {
        const Range<T> input;
        const Range<T> output;
    };

    /// Try to find the given input value in the the entries array. If the value is found, perform scaling and then
    /// return the scaled value. If the value is not found, return std::nullopt.
    template <typename T, const size_t N>
    [[nodiscard]] std::optional<T> find_and_scale_value(const std::array<Entry<T>, N> &entries, const T val)
    {
        auto result = std::find_if(
            entries.begin(), entries.end(), [val](const auto &e) { return val >= e.input.min && val <= e.input.max; });

        if (result != entries.end()) {
            return scale_value(result->input, result->output, val);
        }

        return {};
    }
} // namespace utils
