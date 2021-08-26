// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Profile.hpp"
#include <memory>

extern "C"
{
#include <module-bluetooth/lib/btstack/src/bluetooth.h>
};
namespace bluetooth
{
    enum SCOCodec
    {
        CVSD  = 1,
        mSBC  = 2,
        both  = 3,
        other = 4
    };
    class SCO
    {
      public:
        SCO();
        ~SCO();

        SCO(const SCO &other) = delete;
        auto operator=(SCO rhs) -> SCO & = delete;
        SCO(SCO &&other) noexcept;
        auto operator=(SCO &&other) noexcept -> SCO &;

        void init();
        void send(hci_con_handle_t sco_handle);
        void receive(uint8_t *packet, uint16_t size);
        [[nodiscard]] auto getStreamData() const -> std::shared_ptr<BluetoothStreamData>;
        void setOwnerService(const sys::Service *service);
        void setCodec(SCOCodec codec);

        static constexpr auto CVSD_SAMPLE_RATE = 8000;

      private:
        class SCOImpl;
        std::unique_ptr<SCOImpl> pimpl;
    };
} // namespace bluetooth
