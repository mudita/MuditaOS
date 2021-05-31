// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{
    class AudioVolume : public BluetoothMessage
    {
      public:
        explicit AudioVolume(std::uint8_t volume) : volume{volume}
        {}

        [[nodiscard]] auto getVolume() const noexcept -> std::uint8_t
        {
            return volume;
        }

      private:
        const std::uint8_t volume;
    };

    class A2DPVolume : public AudioVolume
    {
      public:
        explicit A2DPVolume(std::uint8_t volume) : AudioVolume{volume}
        {}
    };
    class HSPVolume : public AudioVolume
    {
      public:
        explicit HSPVolume(std::uint8_t volume) : AudioVolume{volume}
        {}
    };
} // namespace message::bluetooth
