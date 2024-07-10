// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-utils/EventStore/EventStore.hpp>

namespace app
{
    class AbstractUsbStatusModel
    {
      public:
        virtual ~AbstractUsbStatusModel() noexcept = default;

        [[nodiscard]] virtual bool isUsbConnected(const Store::Battery::State &state) const = 0;
    };

    class UsbStatusModel : public AbstractUsbStatusModel
    {
      public:
        [[nodiscard]] bool isUsbConnected(const Store::Battery::State &state) const override;

      private:
        [[nodiscard]] bool isUsbConfigured() const;
    };
} // namespace app
