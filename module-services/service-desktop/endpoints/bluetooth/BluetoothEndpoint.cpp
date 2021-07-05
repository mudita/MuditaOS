// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothEndpoint.hpp"
#include <endpoints/Context.hpp>

using namespace parserFSM;

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
