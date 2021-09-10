// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/update/UpdateEndpoint.hpp>

#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <endpoints/message/Sender.hpp>

#include <json11.hpp>

#include <memory>

namespace sdesktop::endpoints
{

    auto UpdateEndpoint::handle(Context &context) -> void
    {
        auto &p               = helperSwitcher(context);
        auto [sent, response] = p.process(context.getMethod(), context);
        if (sent == sent::delayed) {
            LOG_DEBUG("There is no proper delayed serving mechanism - depend on invisible context caching");
        }
        if (sent == sent::no) {
            if (not response) {
                LOG_ERROR("Response not sent & response not created : respond with error");
                context.setResponseStatus(http::Code::NotAcceptable);
            }
            else {
                context.setResponse(response.value());
            }

            sender::putToSendQueue(context.createSimpleResponse());
        }
        if (sent == sent::yes and response) {
            LOG_ERROR("Response set when we already handled response in handler");
        }
    }

    auto UpdateEndpoint::helperSwitcher(Context &ctx) -> BaseHelper &
    {
        return *updateHelper;
    }

} // namespace sdesktop::endpoints
