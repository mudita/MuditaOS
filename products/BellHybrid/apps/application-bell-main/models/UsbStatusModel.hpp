// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-utils/EventStore/EventStore.hpp>

namespace app
{
    class AbstractUsbStatusModel
    {
      public:
        enum class UsbStatus : bool
        {
            Disconnected,
            Connected
        };

        virtual ~AbstractUsbStatusModel() noexcept = default;

        virtual bool isUsbConnected(const Store::Battery::State &state) const = 0;
        virtual void setUsbStatus(UsbStatus status)                           = 0;
    };

    class UsbStatusModel : public AbstractUsbStatusModel
    {
      public:
        bool isUsbConnected(const Store::Battery::State &state) const override;
        void setUsbStatus(UsbStatus status) override;

      private:
        UsbStatus usbConnected{UsbStatus::Disconnected};
    };
} // namespace app
