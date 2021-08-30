// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "internal/StaticData.hpp"

#include <evtmgr/EventManager.hpp>
#include <evtmgr/messages/AlarmMessage.hpp>
#include <keymap/KeyMap.hpp>
#include <module-bsp/hal/temperature_source/TemperatureSource.hpp>
#include <service-evtmgr/KbdMessage.hpp>

namespace
{
    auto updateTemperature = [](hal::temperature::AbstractTemperatureSource &source) {
        const auto temp = source.read();
        if (not temp) {
            LOG_FATAL("Error during reading from temperature source");
        }
        else {
            evtmgr::internal::StaticData::get().setCurrentTemperature(*temp);
        };
    };
}

EventManager::EventManager(const std::string &name)
    : EventManagerCommon(name), temperatureSource{hal::temperature::AbstractTemperatureSource::Factory::create()}
{
    updateTemperature(*temperatureSource);

    onMinuteTick = [this](const time_t) { updateTemperature(*temperatureSource); };
}

void EventManager::handleKeyEvent(sys::Message *msg)
{
    EventManagerCommon::handleKeyEvent(msg);

    auto kbdMessage = dynamic_cast<sevm::KbdMessage *>(msg);
    if (kbdMessage == nullptr) {
        return;
    }

    auto key = mapKey(static_cast<gui::KeyCode>(kbdMessage->key.keyCode));

    if (key == KeyMap::DeepPressUp && kbdMessage->key.state == RawKey::State::Released) {
        bus.sendMulticast(std::make_unique<AlarmActivated>(), sys::BusChannel::AlarmChanges);
    }
}
