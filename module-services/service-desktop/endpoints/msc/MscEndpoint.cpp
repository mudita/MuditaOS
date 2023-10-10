// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/msc/MscEndpoint.hpp>
#include <endpoints/HttpEnums.hpp>
#include <log/log.hpp>
#include <module-sys/SystemManager/include/SystemManager/SystemManagerCommon.hpp>
#include <endpoints/message/Sender.hpp>

namespace sdesktop::endpoints
{
    auto MscEndpoint::handle(Context &context) -> void
    {
        switch (context.getMethod()) {
        case http::Method::post: {
            // const auto &body = context.getBody(); // probably should parse this somehow but for now any request will
            // do
            context.setResponseStatus(http::Code::OK);

            sender::putToSendQueue(context.createSimpleResponse());

            sys::SystemManagerCommon::RebootMSC(ownerServicePtr);

        } break;
        default:
            LOG_ERROR("Invalid method: %s!", magic_enum::enum_name(context.getMethod()).data());
            context.setResponseStatus(http::Code::BadRequest);
            break;
        }
    }

} // namespace sdesktop::endpoints
