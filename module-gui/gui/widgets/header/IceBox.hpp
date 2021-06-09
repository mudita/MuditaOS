// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>

namespace gui
{
    class IceBox : public HBox
    {
      public:
        explicit IceBox(Item *parent = nullptr);
    };
} // namespace gui
