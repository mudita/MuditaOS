// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EinkMessage.hpp"

#include <gui/Common.hpp>

namespace service::eink
{
    class PrepareDisplayRequest : public EinkMessage
    {
      public:
        explicit PrepareDisplayRequest(::gui::RefreshModes refreshMode);

        [[nodiscard]] auto getRefreshMode() const noexcept -> ::gui::RefreshModes;

      private:
        ::gui::RefreshModes refreshMode;
    };
} // namespace service::eink
