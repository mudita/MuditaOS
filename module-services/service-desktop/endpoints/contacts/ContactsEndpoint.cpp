// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ContactHelper.hpp"
#include "ContactsEndpoint.hpp"

#include <endpoints/Context.hpp>

#include <memory>

using namespace parserFSM;
auto ContactsEndpoint::handle(Context &context) -> void
{
    switch (context.getMethod()) {
    case http::Method::get:
        helper->requestDataFromDB(context);
        break;
    case http::Method::post:
        helper->createDBEntry(context);
        break;
    case http::Method::put:
        helper->updateDBEntry(context);
        break;
    case http::Method::del:
        helper->deleteDBEntry(context);
        break;
    }
}
