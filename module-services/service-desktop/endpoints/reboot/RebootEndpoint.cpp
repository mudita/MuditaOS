﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/reboot/RebootEndpoint.hpp>
#include <endpoints/HttpEnums.hpp>
#include <log/log.hpp>
#include <endpoints/message/Sender.hpp>

namespace sdesktop::endpoints
{
    auto RebootEndpoint::handle(Context &context) -> void
    {
        const auto &[sent, response] = helper->process(context.getMethod(), context);

        if (sent == sent::delayed) {
            LOG_DEBUG("There is no proper delayed serving mechanism - depend on invisible context caching");
        }
        if (sent == sent::no) {
            if (not response.has_value()) {
                LOG_ERROR("Response not sent & response not created : respond with error");
                context.setResponseStatus(http::Code::NotAcceptable);
            }
            else {
                context.setResponse(response.value());
            }

            sender::putToSendQueue(context.createSimpleResponse());
        }
        if (sent == sent::yes and response.has_value()) {
            LOG_ERROR("Response set when we already handled response in handler");
        }
    }

} // namespace sdesktop::endpoints
