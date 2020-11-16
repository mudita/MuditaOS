// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalllogEndpoint.hpp"

#include <endpoints/Context.hpp>

using namespace parserFSM;

auto CalllogEndpoint::handle(Context &context) -> void
{
    switch (context.getMethod()) {
    case http::Method::get:
        helper->requestDataFromDB(context);
        break;
    case http::Method::post: // update entry
        helper->updateDBEntry(context);
        break;
    case http::Method::put:
        helper->createDBEntry(context);
        break;
    case http::Method::del:
        helper->deleteDBEntry(context);
        break;
    }
}
