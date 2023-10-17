// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/bluetooth/BluetoothEventMessages.hpp>

namespace sdesktop::bluetooth
{
    BluetoothEvent::BluetoothEvent()
    {
        context.setResponseStatus(endpoints::http::Code::OK);
        context.setEndpoint(endpoints::EndpointType::bluetooth);
    }

} // namespace sdesktop::bluetooth
