// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Context.hpp"

#include <Service/Service.hpp>
#include <endpoints/message/Sender.hpp>

namespace sdesktop::endpoints
{

    class Endpoint
    {
      public:
        explicit Endpoint(sys::Service *_ownerServicePtr) : ownerServicePtr(_ownerServicePtr){};
        virtual ~Endpoint()                           = default;
        virtual auto handle(Context &context) -> void = 0;
        auto c_str() -> const char *
        {
            return debugName.c_str();
        }

      protected:
        std::string debugName;
        sys::Service *ownerServicePtr = nullptr;
    };

    class SecuredEndpoint : public Endpoint
    {
      public:
        explicit SecuredEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
        {}
        ~SecuredEndpoint() = default;

        auto handle(Context &context) -> void override
        {
            context.setResponseStatus(http::Code::Forbidden);
            sdesktop::endpoints::sender::putToSendQueue(context.createSimpleResponse());
            LOG_INFO("Endpoint #%d secured", static_cast<int>(context.getEndpoint()));
        }
    };

} // namespace sdesktop::endpoints
