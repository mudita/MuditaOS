// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "UpdateHelper.hpp"
#include <endpoints/Endpoint.hpp>
#include <parser/ParserUtils.hpp>

#include <Service/Common.hpp>
#include <Service/Service.hpp>

#include <string>

namespace parserFSM
{
    class Context;
} // namespace parserFSM
namespace sys
{
    class Service;
} // namespace sys

class UpdateEndpoint : public parserFSM::Endpoint
{
  private:
    const std::unique_ptr<parserFSM::UpdateHelper> updateHelper;

  public:
    explicit UpdateEndpoint(sys::Service *ownerServicePtr)
        : Endpoint(ownerServicePtr), updateHelper(std::make_unique<parserFSM::UpdateHelper>(ownerServicePtr))
    {
        debugName = "UpdateEndpoint";
    }
    auto handle(parserFSM::Context &context) -> void override;

    [[nodiscard]] auto helperSwitcher(parserFSM::Context &ctx) -> parserFSM::BaseHelper &;
};
