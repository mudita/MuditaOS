// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ListItem.hpp"

namespace gui
{
    class ListItemWithDescription : public ListItem
    {
      private:
        std::string description;

      public:
        ListItemWithDescription(std::string desc = "") : ListItem(), description(desc){};
        auto getDescription() const noexcept -> std::string;
        auto setDescription(std::string description) -> void;
    };
} /* namespace gui */
