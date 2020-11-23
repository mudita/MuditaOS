// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DeveloperModeHelper.hpp"
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

class DeveloperModeEndpoint : public Endpoint
{
  private:
    std::unique_ptr<DeveloperModeHelper> helper;

  public:
    DeveloperModeEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
    {
        debugName = "DeveloperModeEndpoint";
        helper    = std::make_unique<DeveloperModeHelper>(ownerServicePtr);
    }
    auto handle(Context &context) -> void override;
};
