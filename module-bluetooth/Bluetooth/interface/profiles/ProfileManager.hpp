// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <Error.hpp>
#include "Profile.hpp"
#include "interface/profiles/A2DP/A2DP.hpp"
#include "interface/profiles/HSP/HSP.hpp"

extern "C"
{
#include <bluetooth.h>
#include "btstack_util.h"
}

namespace bluetooth
{

    enum class AudioProfile
    {
        A2DP,
        HSP,
        HFP,
        None
    };

    using ProfileList = std::map<AudioProfile, std::shared_ptr<bluetooth::Profile>>;

    class ProfileManager
    {
      public:
        explicit ProfileManager(sys::Service *ownerService);

        auto init() -> Error::Code;

        auto connect(bd_addr_t address) -> Error::Code;

        auto disconnect() -> Error::Code;

        auto switchProfile(AudioProfile profile) -> Error::Code;

        auto start() -> Error::Code;
        auto stop() -> Error::Code;

      private:
        sys::Service *ownerService;
        ProfileList profilesList;
        bluetooth::Profile *currentProfilePtr = nullptr;
        bd_addr_t remoteAddr{};
        bool initialized = false;
    };
} // namespace bluetooth
