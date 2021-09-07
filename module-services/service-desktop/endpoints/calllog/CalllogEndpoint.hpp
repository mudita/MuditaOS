// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalllogHelper.hpp"
#include <Service/Service.hpp>
#include <endpoints/Endpoint.hpp>
#include <parser/ParserUtils.hpp>

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

class CalllogEndpoint : public parserFSM::Endpoint
{
  private:
    std::unique_ptr<parserFSM::CalllogHelper> helper;

  public:
    explicit CalllogEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
    {
        debugName = "CalllogEndpoint";
        helper    = std::make_unique<parserFSM::CalllogHelper>(ownerServicePtr);
    }
    auto handle(parserFSM::Context &context) -> void override;
};
