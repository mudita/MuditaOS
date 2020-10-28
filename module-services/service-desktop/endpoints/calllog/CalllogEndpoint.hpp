// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory> // for make_unique, unique_ptr
#include <string> // for string

#include "Endpoint.hpp" // for Endpoint
#include "Service/Service.hpp"
#include "CalllogHelper.hpp" // for CalllogHelper
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

class CalllogEndpoint : public Endpoint
{
  private:
    std::unique_ptr<CalllogHelper> helper;

  public:
    CalllogEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
    {
        debugName = "CalllogEndpoint";
        helper    = std::make_unique<CalllogHelper>(ownerServicePtr);
    }
    auto handle(Context &context) -> void override;
};
