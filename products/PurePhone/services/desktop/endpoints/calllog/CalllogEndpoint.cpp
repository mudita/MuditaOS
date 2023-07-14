// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/calllog/CalllogEndpoint.hpp>

namespace sdesktop::endpoints
{
    auto CalllogEndpoint::handle(Context &context) -> void
    {
        switch (context.getMethod()) {
        case http::Method::Get:
            helper->requestDataFromDB(context);
            break;
        case http::Method::Post: // update entry
            helper->updateDBEntry(context);
            break;
        case http::Method::Put:
            helper->createDBEntry(context);
            break;
        case http::Method::Del:
            helper->deleteDBEntry(context);
            break;
        }
    }
} // namespace sdesktop::endpoints
