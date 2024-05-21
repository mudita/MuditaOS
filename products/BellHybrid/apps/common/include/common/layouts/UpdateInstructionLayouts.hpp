// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>
#include <map>
#include <string>

namespace gui
{
    class UpdateInstructionLayoutProvider;
    using LayoutGenerator = std::function<UpdateInstructionLayoutProvider *()>;

    namespace factory
    {
        std::vector<LayoutGenerator> getUpdateInstructionLayouts();
    } // namespace factory
};    // namespace gui
