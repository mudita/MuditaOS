// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AudioFormat.hpp"

#include <integer.hpp>

using audio::AudioFormat;

auto AudioFormat::getSampleRate() const noexcept -> unsigned int
{
    return sampleRate;
}

auto AudioFormat::getBitWidth() const noexcept -> unsigned int
{
    return bitWidth;
}

auto AudioFormat::getChannels() const noexcept -> unsigned int
{
    return channels;
}

auto AudioFormat::getBitrate() const noexcept -> unsigned long int
{
    return sampleRate * bitWidth * channels;
}

auto AudioFormat::toString() const -> std::string
{
    return "AudioFormat{" + std::to_string(sampleRate) + "," + std::to_string(bitWidth) + "," +
           std::to_string(channels) + "}";
}

auto AudioFormat::operator==(const AudioFormat &other) const -> bool
{
    return sampleRate == other.sampleRate && bitWidth == other.bitWidth && channels == other.channels;
}

auto AudioFormat::operator!=(const AudioFormat &other) const -> bool
{
    return !operator==(other);
}

auto AudioFormat::operator>(const AudioFormat &other) const -> bool
{
    return getBitrate() > other.getBitrate();
}

auto AudioFormat::operator<(const AudioFormat &other) const -> bool
{
    return getBitrate() < other.getBitrate();
}

auto AudioFormat::operator<=(const AudioFormat &other) const -> bool
{
    return getBitrate() <= other.getBitrate();
}

auto AudioFormat::operator>=(const AudioFormat &other) const -> bool
{
    return getBitrate() >= other.getBitrate();
}

auto AudioFormat::isValid() const noexcept -> bool
{
    return !(sampleRate == 0 || bitWidth == 0 || channels == 0);
}

auto AudioFormat::makeMatrix(std::set<unsigned int> sampleRates,
                             std::set<unsigned int> bitWidths,
                             std::set<unsigned int> channels) -> std::vector<AudioFormat>
{
    std::vector<AudioFormat> v;

    for (auto sampleRate : sampleRates) {
        for (auto bitWidth : bitWidths) {
            for (auto channelCount : channels) {
                v.push_back(AudioFormat{sampleRate, bitWidth, channelCount});
            }
        }
    }

    return v;
}

auto AudioFormat::isNull() const noexcept -> bool
{
    return operator==(audio::nullFormat);
}

auto AudioFormat::bytesToMicroseconds(unsigned int bytes) const noexcept -> std::chrono::microseconds
{
    auto bytesPerSecond = getBitrate() / utils::integer::BitsInByte;
    auto duration       = std::chrono::duration<double>(static_cast<double>(bytes) / bytesPerSecond);

    return std::chrono::duration_cast<std::chrono::microseconds>(duration);
}

auto AudioFormat::microsecondsToBytes(std::chrono::microseconds duration) const noexcept -> unsigned int
{
    auto bytesPerSecond = getBitrate() / utils::integer::BitsInByte;
    auto seconds        = std::chrono::duration<double>(duration).count();
    return static_cast<unsigned int>(seconds * bytesPerSecond);
}
