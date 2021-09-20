// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// Copyright (c) 2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AudioDevice.hpp"

#include <Audio/Profiles/Profile.hpp>

#include <memory>

namespace audio
{

    class AudioDeviceFactory
    {
      public:
        class Observer
        {
          public:
            virtual void onDeviceCreated(std::shared_ptr<AudioDevice> device, AudioDevice::Type deviceType) = 0;
        };

        explicit AudioDeviceFactory(Observer *observer = nullptr);
        virtual ~AudioDeviceFactory() = default;

        void setObserver(Observer *observer) noexcept;
        std::shared_ptr<AudioDevice> CreateDevice(const Profile &profile);
        virtual std::shared_ptr<AudioDevice> createCellularAudioDevice() = 0;

      protected:
        virtual std::shared_ptr<AudioDevice> getDevice(const Profile &profile) = 0;

      private:
        Observer *_observer = nullptr;
    };

}; // namespace audio
