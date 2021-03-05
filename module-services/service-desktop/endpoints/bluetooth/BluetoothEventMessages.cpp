// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothEventMessages.hpp"

namespace sdesktop::bluetooth
{
    using namespace parserFSM;

    BluetoothEvent::BluetoothEvent()
    {
        context.setResponseStatus(http::Code::OK);
        context.setEndpoint(EndpointType::bluetooth);
    }

} // namespace sdesktop::bluetooth
