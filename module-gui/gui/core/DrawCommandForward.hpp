// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <list>

namespace gui
{
    class DrawCommand;

    using Command = std::unique_ptr<DrawCommand>;

    using Commands = std::list<Command>;

} // namespace gui
