// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include "Result.hpp"
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
        virtual auto init() -> Result::Code                                                       = 0;
        virtual void deInit()                                                                    = 0;
        virtual auto connect(const Devicei &device) -> Result::Code                               = 0;
        virtual auto disconnect() -> Result::Code                                                 = 0;
        virtual auto start() -> Result::Code                                                      = 0;
        virtual auto stop() -> Result::Code                                                       = 0;
        virtual auto incomingCallStarted() -> Result::Code                                        = 0;
        virtual auto outgoingCallStarted(const utils::PhoneNumber &) -> Result::Code              = 0;
        virtual auto incomingCallAnswered() -> Result::Code                                       = 0;
        virtual auto outgoingCallAnswered() -> Result::Code                                       = 0;
        virtual auto callTerminated() -> Result::Code                                             = 0;
        virtual auto callMissed() -> Result::Code                                                 = 0;
        virtual auto setIncomingCallNumber(const utils::PhoneNumber &nr) -> Result::Code          = 0;
        virtual auto setSignalStrengthData(const DataVariant &data) -> Result::Code               = 0;
        virtual auto setOperatorNameData(const DataVariant &data) -> Result::Code                 = 0;
        virtual auto setBatteryLevelData(unsigned int) -> Result::Code                            = 0;
        virtual auto setNetworkStatusData(const DataVariant &data) -> Result::Code                = 0;
        virtual auto setAudioDevice(std::shared_ptr<BluetoothAudioDevice> device) -> Result::Code = 0;
    };

    class ProfileManager : public BaseProfileManager
    {
      public:
        explicit ProfileManager(sys::Service *ownerService);
        ProfileManager() = delete;

        auto init() -> Result::Code override;
        void deInit() override;
        auto connect(const Devicei &device) -> Result::Code override;
        auto disconnect() -> Result::Code override;
        auto start() -> Result::Code override;
        auto stop() -> Result::Code override;
        auto incomingCallStarted() -> Result::Code override;
        auto outgoingCallStarted(const utils::PhoneNumber &nr) -> Result::Code override;
        auto incomingCallAnswered() -> Result::Code override;
        auto outgoingCallAnswered() -> Result::Code override;
        auto callTerminated() -> Result::Code override;
        auto callMissed() -> Result::Code override;
        auto setIncomingCallNumber(const utils::PhoneNumber &nr) -> Result::Code override;
        auto setSignalStrengthData(const DataVariant &data) -> Result::Code override;
        auto setOperatorNameData(const DataVariant &data) -> Result::Code override;
        auto setBatteryLevelData(unsigned int level) -> Result::Code override;
        auto setNetworkStatusData(const DataVariant &data) -> Result::Code override;

        auto setAudioDevice(std::shared_ptr<BluetoothAudioDevice> device) -> Result::Code override;

      private:
        sys::Service *ownerService;
        ProfileList profilesList;
        bluetooth::MusicProfile *musicProfilePtr = nullptr;
        bluetooth::CallProfile *callProfilePtr   = nullptr;
        bool initialized                         = false;
    };
} // namespace bluetooth
