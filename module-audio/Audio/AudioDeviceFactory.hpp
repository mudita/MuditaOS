// Copyright (c) 2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AudioDevice.hpp"

#include <memory>

namespace audio
{

    class AudioDeviceFactory
    {
        class Observer
        {
          public:
            virtual void onDeviceCreated(std::shared_ptr<AudioDevice> device) = 0;
        };

      public:
        explicit AudioDeviceFactory(Observer *observer = nullptr);

        void setObserver(Observer *observer) noexcept;
        std::shared_ptr<AudioDevice> CreateDevice(AudioDevice::Type);

      protected:
        virtual std::shared_ptr<AudioDevice> getDeviceFromType(AudioDevice::Type) = 0;

      private:
        Observer *_observer = nullptr;
    };

}; // namespace audio
