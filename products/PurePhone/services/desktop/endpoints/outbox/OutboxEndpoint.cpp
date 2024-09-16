// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <endpoints/outbox/OutboxEndpoint.hpp>

namespace sdesktop::endpoints
{
    auto OutboxEndpoint::handle(Context &context) -> void
    {
        switch (context.getMethod()) {
        case http::Method::Get:
            helper->processGetRequest(context);
            break;
        case http::Method::Del:
            helper->processDeleteRequest(context);
            break;
        default:
            context.setResponseStatus(http::Code::BadRequest);
            break;
        }
    }
} // namespace sdesktop::endpoints
