// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "EinkMessage.hpp"

#include <gui/Common.hpp>

namespace service::eink
{
    class PrepareDisplayEarlyRequest : public EinkMessage
    {
      public:
        explicit PrepareDisplayEarlyRequest(::gui::RefreshModes refreshMode);

        [[nodiscard]] auto getRefreshMode() const noexcept -> ::gui::RefreshModes;

      private:
        ::gui::RefreshModes refreshMode;
    };
} // namespace service::eink
