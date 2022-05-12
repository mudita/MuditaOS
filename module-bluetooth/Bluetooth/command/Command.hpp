// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "CommandData.hpp"
#include "event/Events.hpp"

namespace bluetooth
{

    class Command
    {
      public:
        struct CommandPack
        {
            std::unique_ptr<bt::evt::Base> evt;
        };
        CommandPack pack;
    };

} // namespace bluetooth
