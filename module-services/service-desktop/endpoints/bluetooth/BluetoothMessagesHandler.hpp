// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <memory>
#include <service-bluetooth/messages/ResponseVisibleDevices.hpp>
#include <service-bluetooth/messages/BondedDevices.hpp>
#include <service-bluetooth/messages/Status.hpp>

namespace sys
{
    class Service;
}

namespace sdesktop::developerMode
{
    class DeveloperModeRequest;
}

namespace sdesktop::bluetooth
{
    class ResponseEvent;
    class BluetoothMessagesHandler
    {
        sys::Service *ownerService = nullptr;

        void send(std::unique_ptr<ResponseEvent> event);

      public:
        BluetoothMessagesHandler(sys::Service *ownerService);

        [[nodiscard]] auto handle(message::bluetooth::ResponseStatus *msg) -> std::shared_ptr<sys::ResponseMessage>;
        [[nodiscard]] auto handle(message::bluetooth::ResponseBondedDevices *msg)
            -> std::shared_ptr<sys::ResponseMessage>;
        [[nodiscard]] auto handle(message::bluetooth::ResponseVisibleDevices *msg)
            -> std::shared_ptr<sys::ResponseMessage>;
    };
} // namespace sdesktop::bluetooth
