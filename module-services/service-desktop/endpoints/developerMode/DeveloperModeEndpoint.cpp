// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeveloperModeEndpoint.hpp"

#include "Context.hpp" // for Context

using namespace parserFSM;

auto DeveloperModeEndpoint::handle(Context &context) -> void
{
    switch (context.getMethod()) {
    case http::Method::get:
        break;
    case http::Method::post:
        break;
    case http::Method::put:
        helper->processPutRequest(context);
        break;
    case http::Method::del:
        break;
    }
}
