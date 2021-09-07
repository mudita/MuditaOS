// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <chrono>
#include <set>
#include <string>
#include <vector>

namespace audio
{

    class AudioFormat
    {
      private:
        unsigned int sampleRate = 0;
        unsigned int bitWidth   = 0;
        unsigned int channels   = 0;

      public:
        constexpr AudioFormat() = default;
        constexpr AudioFormat(unsigned int sampleRate, unsigned int bitWidth, unsigned int channels) noexcept
            : sampleRate(sampleRate), bitWidth(bitWidth), channels(channels)
        {}

        auto getSampleRate() const noexcept -> unsigned int;
        auto getBitWidth() const noexcept -> unsigned int;
        auto getChannels() const noexcept -> unsigned int;

        auto getBitrate() const noexcept -> unsigned long int;
        auto toString() const -> std::string;

        auto bytesToMicroseconds(unsigned int bytes) const noexcept -> std::chrono::microseconds;
        auto microsecondsToBytes(std::chrono::microseconds duration) const noexcept -> unsigned int;

        auto operator==(const AudioFormat &other) const -> bool;
        auto operator!=(const AudioFormat &other) const -> bool;
        auto operator>(const AudioFormat &other) const -> bool;
        auto operator<(const AudioFormat &other) const -> bool;
        auto operator<=(const AudioFormat &other) const -> bool;
        auto operator>=(const AudioFormat &other) const -> bool;

        auto isValid() const noexcept -> bool;
        auto isNull() const noexcept -> bool;

        static auto makeMatrix(std::set<unsigned int> sampleRates,
                               std::set<unsigned int> bitWidths,
                               std::set<unsigned int> channels) -> std::vector<AudioFormat>;
    };

    static constexpr inline auto nullFormat = AudioFormat();

}; // namespace audio
