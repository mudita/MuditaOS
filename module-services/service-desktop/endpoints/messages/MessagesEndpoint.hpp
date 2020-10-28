// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory> // for make_shared, shared_ptr
#include <string> // for string

#include "Endpoint.hpp" // for Endpoint
#include "Service/Service.hpp"
#include "MessageHelper.hpp" // for MessageHelper
#include "ParserUtils.hpp"   // for parserFSM

namespace parserFSM
{
    class Context;
} // namespace parserFSM
namespace sys
{
    class Service;
} // namespace sys

using namespace parserFSM;

class MessagesEndpoint : public Endpoint
{
  private:
    std::shared_ptr<MessageHelper> helper;

  public:
    MessagesEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
    {
        debugName = "MessagesEndpoint";
        helper    = std::make_shared<MessageHelper>(ownerServicePtr);
    }
    auto handle(Context &context) -> void override;
};
