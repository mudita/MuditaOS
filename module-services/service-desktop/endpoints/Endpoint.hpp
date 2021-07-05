// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Context.hpp"
#include <parser/ParserUtils.hpp>
#include <parser/MessageHandler.hpp>

#include <json11.hpp>
#include <Common/Query.hpp>
#include <Service/Service.hpp>
#include <string>
#include <memory>

namespace parserFSM
{

    class Endpoint
    {
      public:
        explicit Endpoint(sys::Service *_ownerServicePtr) : ownerServicePtr(_ownerServicePtr){};
        virtual ~Endpoint()                                      = default;
        virtual auto handle(parserFSM::Context &context) -> void = 0;
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
            MessageHandler::putToSendQueue(context.createSimpleResponse());
            LOG_INFO("Endpoint #%d secured", static_cast<int>(context.getEndpoint()));
        }
    };

} // namespace parserFSM
