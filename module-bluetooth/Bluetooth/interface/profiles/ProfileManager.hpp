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

    class BaseProfileManager
    {
      public:
        virtual ~BaseProfileManager()                                                            = default;
        virtual auto init() -> Error::Code                                                       = 0;
        virtual auto connect(const Devicei &device) -> Error::Code                               = 0;
        virtual auto disconnect() -> Error::Code                                                 = 0;
        virtual auto start() -> Error::Code                                                      = 0;
        virtual auto stop() -> Error::Code                                                       = 0;
        virtual auto startRinging() -> Error::Code                                               = 0;
        virtual auto stopRinging() -> Error::Code                                                = 0;
        virtual auto initializeCall() -> Error::Code                                             = 0;
        virtual auto terminateCall() -> Error::Code                                              = 0;
        virtual auto callAnswered() -> Error::Code                                               = 0;
        virtual auto callStarted(const utils::PhoneNumber &) -> Error::Code                      = 0;
        virtual auto setIncomingCallNumber(const utils::PhoneNumber &nr) -> Error::Code          = 0;
        virtual auto setSignalStrengthData(const DataVariant &data) -> Error::Code               = 0;
        virtual auto setOperatorNameData(const DataVariant &data) -> Error::Code                 = 0;
        virtual auto setBatteryLevelData(unsigned int) -> Error::Code                            = 0;
        virtual auto setNetworkStatusData(const DataVariant &data) -> Error::Code                = 0;
        virtual auto setAudioDevice(std::shared_ptr<BluetoothAudioDevice> device) -> Error::Code = 0;
    };

    class ProfileManager : public BaseProfileManager
    {
      public:
        explicit ProfileManager(sys::Service *ownerService);
        ProfileManager() = delete;

        auto init() -> Error::Code override;
        auto connect(const Devicei &device) -> Error::Code override;
        auto disconnect() -> Error::Code override;
        auto start() -> Error::Code override;
        auto stop() -> Error::Code override;
        auto startRinging() -> Error::Code override;
        auto stopRinging() -> Error::Code override;
        auto initializeCall() -> Error::Code override;
        auto terminateCall() -> Error::Code override;
        auto callAnswered() -> Error::Code override;
        auto callStarted(const utils::PhoneNumber &) -> Error::Code override;
        auto setIncomingCallNumber(const utils::PhoneNumber &nr) -> Error::Code override;
        auto setSignalStrengthData(const DataVariant &data) -> Error::Code override;
        auto setOperatorNameData(const DataVariant &data) -> Error::Code override;
        auto setBatteryLevelData(unsigned int) -> Error::Code override;
        auto setNetworkStatusData(const DataVariant &data) -> Error::Code override;

        auto setAudioDevice(std::shared_ptr<BluetoothAudioDevice> device) -> Error::Code override;

      private:
        sys::Service *ownerService;
        ProfileList profilesList;
        bluetooth::MusicProfile *musicProfilePtr = nullptr;
        bluetooth::CallProfile *callProfilePtr   = nullptr;
        bool initialized = false;
    };
} // namespace bluetooth
