// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/Endpoint.hpp>
#include <endpoints/message/Sender.hpp>
#include <log/log.hpp>

namespace sdesktop::endpoints
{

    Endpoint::Endpoint(sys::Service *_ownerServicePtr) : ownerServicePtr(_ownerServicePtr)
    {}

    const char *Endpoint::c_str()
    {
        return debugName.c_str();
    }

    SecuredEndpoint::SecuredEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {}

    void SecuredEndpoint::handle(Context &context)
    {
        context.setResponseStatus(http::Code::Forbidden);
        sender::putToSendQueue(context.createSimpleResponse());
        LOG_INFO("Endpoint '%s' secured", magic_enum::enum_name(context.getEndpoint()).data());
    }

} // namespace sdesktop::endpoints
