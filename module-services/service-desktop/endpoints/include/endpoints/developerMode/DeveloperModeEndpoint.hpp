// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include "DeveloperModeHelper.hpp"
#include "Mode/UI_Helper.hpp"
#include "fs/FS_Helper.hpp"

#include <memory>

namespace sdesktop::endpoints
{

    class DeveloperModeEndpoint : public Endpoint
    {
      private:
        const std::unique_ptr<DeveloperModeHelper> helper;
        const std::unique_ptr<UI_Helper> uiHelper;
        const std::unique_ptr<FS_Helper> fsHelper;

      public:
        explicit DeveloperModeEndpoint(sys::Service *_ownerServicePtr)
            : Endpoint(_ownerServicePtr), helper(std::make_unique<DeveloperModeHelper>(ownerServicePtr)),
              uiHelper(std::make_unique<UI_Helper>(ownerServicePtr)),
              fsHelper(std::make_unique<FS_Helper>(ownerServicePtr))
        {
            debugName = "DeveloperModeEndpoint";
        }

        auto handle(Context &context) -> void override;

        [[nodiscard]] auto helperSwitcher(Context &ctx) -> BaseHelper &;
    };

} // namespace sdesktop::endpoints
