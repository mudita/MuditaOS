// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/Endpoint.hpp>
#include <endpoints/message/Sender.hpp>

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
        LOG_INFO("Endpoint #%d secured", static_cast<int>(context.getEndpoint()));
    }

} // namespace sdesktop::endpoints
