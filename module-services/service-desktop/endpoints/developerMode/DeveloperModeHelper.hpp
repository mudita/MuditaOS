// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-bsp/bsp/keyboard/key_codes.hpp>
#include <module-services/service-evtmgr/messages/KbdMessage.hpp>
#include "Common/Query.hpp"
#include "Context.hpp"
#include "Service/Service.hpp"
#include "Service/Common.hpp" // for ReturnCodes

namespace sys
{
    class Service;
} // namespace sys

namespace parserFSM
{

    class DeveloperModeHelper
    {
        sys::Service *ownerServicePtr = nullptr;
        static auto getKeyCode(int val) noexcept -> bsp::KeyCodes;
        void sendKeypress(bsp::KeyCodes keyCode);

      public:
        DeveloperModeHelper(sys::Service *_ownerServicePtr) : ownerServicePtr(_ownerServicePtr){};
        auto processPutRequest(Context &context) -> sys::ReturnCodes;
    };

    namespace json::developerMode
    {
        const inline std::string keyPressed = "keyPressed";
    }
} // namespace parserFSM
