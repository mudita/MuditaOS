// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace app
{
    enum class StartupReason
    {
        Launch,   // Default startup causing application MainWindow to be added to stack.
        OnAction, // Switch to application  was caused by action. Enum is used to prevent called applications to
        // switch to main window on application switch and allow declared handler to switch to desired window.
    };
}
