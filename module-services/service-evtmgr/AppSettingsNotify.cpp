// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Service/BusProxy.hpp"
#include <application-settings/ApplicationSettings.hpp>

namespace evm::api
{
    void notifySettingsBluetoothAudio(sys::BusProxy &bus, std::shared_ptr<audio::Event> event)
    {
        switch (event->getType()) {
        case audio::EventType::BluetoothA2DPDeviceState: {
            auto message = std::make_shared<message::bluetooth::ProfileStatus>(
                bluetooth::AudioProfile::A2DP, (event->getDeviceState() == audio::Event::DeviceState::Connected));
            bus.sendUnicast(message, app::name_settings);
        } break;
        case audio::EventType::BluetoothHSPDeviceState: {
            auto message = std::make_shared<message::bluetooth::ProfileStatus>(
                bluetooth::AudioProfile::HSP, (event->getDeviceState() == audio::Event::DeviceState::Connected));
            bus.sendUnicast(message, app::name_settings);
        } break;
        case audio::EventType::BluetoothHFPDeviceState: {
            auto message = std::make_shared<message::bluetooth::ProfileStatus>(
                bluetooth::AudioProfile::HFP, (event->getDeviceState() == audio::Event::DeviceState::Connected));
            bus.sendUnicast(message, app::name_settings);
        } break;
        default:
            break;
        }
    }
} // namespace evm::api
