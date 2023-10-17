// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FreeRTOS.h"
#include "Profile.hpp"
#include "Service/Service.hpp"
#include "queue.h"
#include <memory>

namespace bluetooth
{
    class A2DP : public MusicProfile
    {
      public:
        A2DP();
        ~A2DP() override;

        A2DP(A2DP &other);
        auto operator=(A2DP rhs) -> A2DP &;
        A2DP(A2DP &&other) noexcept;
        auto operator=(A2DP &&other) noexcept -> A2DP &;

        auto init() -> Result::Code override;
        void setDevice(const Devicei &device) override;
        void setOwnerService(sys::Service *service) override;

        void connect() override;
        void disconnect() override;
        void start() override;
        void stop() override;
        void setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice) override;

      private:
        class A2DPImpl;
        std::unique_ptr<A2DPImpl> pimpl;
    };
} // namespace bluetooth
