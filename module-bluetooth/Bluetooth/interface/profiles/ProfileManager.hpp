// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <Error.hpp>
#include "Profile.hpp"
#include "AudioProfile.hpp"
#include "interface/profiles/A2DP/A2DP.hpp"
#include "interface/profiles/HSP/HSP.hpp"
#include "interface/profiles/HFP/HFP.hpp"
#include "audio/BluetoothAudioDevice.hpp"

#include <memory>
#include <command/CommandData.hpp>

extern "C"
{
#include <bluetooth.h>
#include "btstack_util.h"
}

namespace bluetooth
{

    using ProfileList = std::map<AudioProfile, std::shared_ptr<bluetooth::Profile>>;

    class ProfileManager
    {
      public:
        explicit ProfileManager(sys::Service *ownerService);

        auto init() -> Error::Code;

        auto connect(const Devicei &device) -> Error::Code;

        auto disconnect() -> Error::Code;

        auto switchProfile(AudioProfile profile) -> Error::Code;

        auto start() -> Error::Code;
        auto stop() -> Error::Code;
        auto startRinging() -> Error::Code;
        auto stopRinging() -> Error::Code;
        auto initializeCall() -> Error::Code;
        auto callAnswered() -> Error::Code;
        auto setIncomingCallNumber(const DataVariant &data) -> Error::Code;
        auto setSignalStrengthData(const DataVariant &data) -> Error::Code;
        auto setOperatorNameData(const DataVariant &data) -> Error::Code;

        auto setAudioDevice(std::shared_ptr<BluetoothAudioDevice> device) -> Error::Code;

      private:
        sys::Service *ownerService;
        ProfileList profilesList;
        bluetooth::Profile *currentProfilePtr = nullptr;
        bool initialized = false;
    };
} // namespace bluetooth
