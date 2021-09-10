// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/messages/MessagesEndpoint.hpp>

namespace sdesktop::endpoints
{

    auto MessagesEndpoint::handle(Context &context) -> void
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

} // namespace sdesktop::endpoints
