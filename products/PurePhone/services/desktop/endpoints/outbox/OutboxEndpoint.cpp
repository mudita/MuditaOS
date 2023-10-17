// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/outbox/OutboxEndpoint.hpp>

namespace sdesktop::endpoints
{

    auto OutboxEndpoint::handle(Context &context) -> void
    {

        switch (context.getMethod()) {
        case http::Method::get:
            helper->processGetRequest(context);
            break;
        case http::Method::del:
            helper->processDeleteRequest(context);
            break;
        default:
            context.setResponseStatus(http::Code::BadRequest);
            break;
        }
    }

} // namespace sdesktop::endpoints
