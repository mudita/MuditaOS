// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>

namespace gui::header
{
    struct DeleteAction : HBox
    {
        explicit DeleteAction(Item *parent = nullptr);
    };
} // namespace gui::header
