﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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

using namespace parserFSM;

class ContactsEndpoint : public Endpoint
{
  private:
    std::unique_ptr<ContactHelper> helper;

  public:
    ContactsEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
    {
        debugName = "ContactsEndpoint";
        helper    = std::make_unique<ContactHelper>(ownerServicePtr);
    }
    auto handle(Context &context) -> void override;
};
