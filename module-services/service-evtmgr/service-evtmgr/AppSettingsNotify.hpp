// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Audio/AudioCommon.hpp"
#include <memory>

namespace sys
{
    class BusProxy;
}

namespace evm::api
{
    void notifySettingsBluetoothAudio(sys::BusProxy &bus, std::shared_ptr<audio::Event> event);
} // namespace evm::api
