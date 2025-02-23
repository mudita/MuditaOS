﻿// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <endpoints/restore/RestoreEndpoint.hpp>
#include <endpoints/message/Sender.hpp>
#include <log/log.hpp>

namespace sdesktop::endpoints
{
    auto RestoreEndpoint::handle(Context &context) -> void
    {
        auto [sent, response] = helper->process(context.getMethod(), context);

        if (sent == Sent::Delayed) {
            LOG_DEBUG("There is no proper delayed serving mechanism - depend on invisible context caching");
        }
        if (sent == Sent::No) {
            if (not response) {
                LOG_ERROR("Response not sent & response not created : respond with error");
                context.setResponseStatus(http::Code::NotAcceptable);
            }
            else {
                context.setResponse(response.value());
            }

            sender::putToSendQueue(context.createSimpleResponse());
        }
        if (sent == Sent::Yes and response) {
            LOG_ERROR("Response set when we already handled response in handler");
        }
    }
} // namespace sdesktop::endpoints
