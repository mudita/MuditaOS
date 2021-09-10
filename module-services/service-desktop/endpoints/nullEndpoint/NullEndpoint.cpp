// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/nullEndpoint/NullEndpoint.hpp>
#include <endpoints/message/Sender.hpp>

namespace sdesktop::endpoints
{

    auto NullEndpoint::handle(Context &context) -> void
    {
        context.setResponseStatus(http::Code::BadRequest);
        sender::putToSendQueue(context.createSimpleResponse());
        LOG_DEBUG("Unknown Endpoint #%d", static_cast<int>(context.getEndpoint()));
    }

} // namespace sdesktop::endpoints
