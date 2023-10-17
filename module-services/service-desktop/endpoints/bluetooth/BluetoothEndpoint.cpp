// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/bluetooth/BluetoothEndpoint.hpp>

namespace sdesktop::endpoints
{

    auto BluetoothEndpoint::handle(Context &context) -> void
    {
        switch (context.getMethod()) {
        case http::Method::get:
            helper->processGetRequest(context);
            break;
        case http::Method::post:
            helper->processPostRequest(context);
            break;
        case http::Method::put:
            helper->processPutRequest(context);
            break;
        case http::Method::del:
            helper->processDeleteRequest(context);
            break;
        }
    }

} // namespace sdesktop::endpoints
