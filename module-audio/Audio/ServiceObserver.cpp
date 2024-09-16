// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "ServiceObserver.hpp"
#include "AudioCommon.hpp"
#include "AudioDevice.hpp"

using namespace audio;

ServiceObserver::ServiceObserver(AudioServiceMessage::Callback serviceCallback)
    : serviceCallback(std::move(serviceCallback))
{}

void ServiceObserver::onDeviceCreated(std::shared_ptr<AudioDevice> device, AudioDevice::Type deviceType)
{
    auto msg = AudioServiceMessage::AudioDeviceCreated(std::move(device), deviceType);
    serviceCallback(&msg);
}
