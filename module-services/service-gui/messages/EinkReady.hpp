// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GUIMessage.hpp"

#include <gui/Common.hpp>

namespace service::gui
{
    class EinkReady : public GUIMessage
    {
      public:
        explicit EinkReady(::gui::Size displaySize) : einkDisplaySize{displaySize}
        {}

        [[nodiscard]] auto getDisplaySize() const noexcept -> ::gui::Size
        {
            return einkDisplaySize;
        }

      private:
        ::gui::Size einkDisplaySize;
    };
} // namespace service::gui
