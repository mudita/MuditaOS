// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DeveloperModeHelper.hpp"
#include "Mode/UI_Helper.hpp"
#include "fs/FS_Helper.hpp"
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

class DeveloperModeEndpoint : public parserFSM::Endpoint
{
  private:
    const std::unique_ptr<parserFSM::DeveloperModeHelper> helper;
    const std::unique_ptr<parserFSM::UI_Helper> uiHelper;
    const std::unique_ptr<parserFSM::FS_Helper> fsHelper;

  public:
    explicit DeveloperModeEndpoint(sys::Service *_ownerServicePtr)
        : Endpoint(_ownerServicePtr), helper(std::make_unique<parserFSM::DeveloperModeHelper>(ownerServicePtr)),
          uiHelper(std::make_unique<parserFSM::UI_Helper>(ownerServicePtr)),
          fsHelper(std::make_unique<parserFSM::FS_Helper>(ownerServicePtr))
    {
        debugName = "DeveloperModeEndpoint";
    }

    auto handle(parserFSM::Context &context) -> void override;

    [[nodiscard]] auto helperSwitcher(parserFSM::Context &ctx) -> parserFSM::BaseHelper &;
};
