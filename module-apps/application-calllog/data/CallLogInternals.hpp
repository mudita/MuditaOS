/*
 * @file CallLogInternals.hpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 8.10.2019
 * @brief Call Log Call Type
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
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
        const inline std::string MainWindowStr       = gui::name::window::main_window;
        const inline std::string DetailsWindowStr    = "DetailsWindow";
        const inline std::string DialogYesNoStr      = "DialogYesNo";

    } // namespace settings
} // namespace calllog
