// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include "CallLogStyle.hpp"
#include "CalllogRecord.hpp"
#include <string>

namespace calllog
{

    enum CallLogCallType
    {
        IN     = 0,
        OUT    = 1,
        MISSED = 2,
        NUM_OF_CALL_TYPES
    };

    inline CallLogCallType toCallLogCallType(const CallType type)
    {
        switch (type) {
        case CallType::CT_INCOMING:
            return CallLogCallType::IN;
        case CallType::CT_OUTGOING:
            return CallLogCallType::OUT;
        default: /* rest */
            return CallLogCallType::MISSED;
        }
    }

    namespace settings
    {
        // Windows
        inline constexpr auto MainWindowStr     = gui::name::window::main_window;
        inline constexpr auto DetailsWindowStr  = "DetailsWindow";
        inline constexpr auto DialogYesNoStr    = "DialogYesNo";
        inline constexpr auto CallLogOptionsStr = "CallLogOptions";

    } // namespace settings
} // namespace calllog
