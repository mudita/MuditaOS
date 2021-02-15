// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Profile.hpp"
#include <service-bluetooth/BluetoothMessage.hpp>

namespace bluetooth
{

    class HSP : public Profile
    {
        static constexpr auto CLASS_OF_DEVICE = 0x400204;
        // Service class: Telephony, Major device class: Phone, Minor device class: Cellular
      public:
        HSP();
        ~HSP() override;

        HSP(const HSP &other) = delete;
        auto operator=(const HSP &rhs) -> HSP & = delete;
        HSP(HSP &&other) noexcept;
        auto operator=(HSP &&other) noexcept -> HSP &;

        auto init() -> Error::Code override;
        void setDeviceAddress(uint8_t *addr) override;
        auto getStreamData() -> std::shared_ptr<BluetoothStreamData> override;
        void setOwnerService(const sys::Service *service) override;

        void connect() override;
        void disconnect() override;

      private:
        class HSPImpl;
        std::unique_ptr<HSPImpl> pimpl;
        const sys::Service *ownerService;
    };

} // namespace Bt
