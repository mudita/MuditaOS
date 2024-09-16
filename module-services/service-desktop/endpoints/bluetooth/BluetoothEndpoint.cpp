// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <endpoints/bluetooth/BluetoothEndpoint.hpp>

namespace sdesktop::endpoints
{
    auto BluetoothEndpoint::handle(Context &context) -> void
    {
        switch (context.getMethod()) {
        case http::Method::Get:
            helper->processGetRequest(context);
            break;
        case http::Method::Post:
            helper->processPostRequest(context);
            break;
        case http::Method::Put:
            helper->processPutRequest(context);
            break;
        case http::Method::Del:
            helper->processDeleteRequest(context);
            break;
        }
    }
} // namespace sdesktop::endpoints
