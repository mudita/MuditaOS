// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Rect.hpp"

namespace gui
{

    class SideListItem : public Rect
    {
      private:
        std::string description;

      public:
        explicit SideListItem(std::string description = "");
        [[nodiscard]] auto getDescription() const noexcept -> std::string;
        auto setDescription(std::string description) -> void;
    };

} /* namespace gui */
