// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ContactHelper.hpp"

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

class ContactsEndpoint : public parserFSM::Endpoint
{
  private:
    std::unique_ptr<parserFSM::ContactHelper> helper;

  public:
    explicit ContactsEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
    {
        debugName = "ContactsEndpoint";
        helper    = std::make_unique<parserFSM::ContactHelper>(ownerServicePtr);
    }
    auto handle(parserFSM::Context &context) -> void override;
};
