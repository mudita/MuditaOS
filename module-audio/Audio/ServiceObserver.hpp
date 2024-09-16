// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "AudioCommon.hpp"
#include "AudioDevice.hpp"
#include "AudioDeviceFactory.hpp"

#include <memory>

namespace audio
{

    class ServiceObserver : public AudioDeviceFactory::Observer
    {
      public:
        explicit ServiceObserver(AudioServiceMessage::Callback serviceCallback);
        virtual void onDeviceCreated(std::shared_ptr<AudioDevice> device, AudioDevice::Type deviceType) final;

      private:
        AudioServiceMessage::Callback serviceCallback;
    };

} // namespace audio
