// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"
#include <Bluetooth/interface/profiles/AudioProfile.hpp>
namespace message::bluetooth
{

    class RequestStatus : public BluetoothMessage
    {};

    class ResponseStatus : public BluetoothMessage
    {
      public:
        explicit ResponseStatus(BluetoothStatus status) : status(status)
        {}
        [[nodiscard]] auto getStatus() const noexcept -> BluetoothStatus
        {
            return status;
        }

      private:
        BluetoothStatus status;
    };

    class ProfileStatus : public BluetoothMessage
    {
      public:
        ProfileStatus(::bluetooth::AudioProfile profile, bool isConnected) : isConnected(isConnected), profile(profile)
        {}

        [[nodiscard]] auto getProfile() const noexcept -> ::bluetooth::AudioProfile
        {
            return profile;
        }
        [[nodiscard]] auto isProfileConnected() const noexcept -> bool
        {
            return isConnected;
        }

      private:
        bool isConnected;
        ::bluetooth::AudioProfile profile;
    };
} // namespace message::bluetooth
