// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MessageHelper.hpp"
#include <endpoints/Endpoint.hpp>
#include <parser/ParserUtils.hpp>

#include <Service/Service.hpp>

#include <memory>
#include <string>

namespace parserFSM
{
    class Context;
} // namespace parserFSM
namespace sys
{
    class Service;
} // namespace sys

class MessagesEndpoint : public parserFSM::Endpoint
{
  private:
    std::shared_ptr<parserFSM::MessageHelper> helper;

  public:
    explicit MessagesEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
    {
        debugName = "MessagesEndpoint";
        helper    = std::make_shared<parserFSM::MessageHelper>(ownerServicePtr);
    }
    auto handle(parserFSM::Context &context) -> void override;
};
