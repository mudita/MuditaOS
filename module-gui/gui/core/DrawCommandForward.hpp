// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>

namespace gui
{
    class DrawCommand;

    using Command = std::unique_ptr<DrawCommand>;
} // namespace gui
