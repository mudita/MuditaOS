// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Context.hpp>

#include <Common/Query.hpp>
#include <Service/Common.hpp>
#include <Service/Service.hpp>
#include <bsp/keyboard/key_codes.hpp>
#include <input/InputEvent.hpp>

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
        void sendKeypress(bsp::KeyCodes keyCode, gui::InputEvent::State state);

        void requestSimChange(const int simSelected);

      public:
        DeveloperModeHelper(sys::Service *_ownerServicePtr) : ownerServicePtr(_ownerServicePtr){};
        auto processPutRequest(Context &context) -> sys::ReturnCodes;
        auto processGetRequest(Context &context) -> sys::ReturnCodes;
    };

    namespace json::developerMode
    {
        inline constexpr auto keyPressed    = "keyPressed";
        inline constexpr auto state         = "state";
        inline constexpr auto systemStarted = "systemStarted";
        inline constexpr auto ATResponse    = "ATResponse";
        inline constexpr auto AT            = "AT";
        inline constexpr auto focus         = "focus";
        inline constexpr auto isLocked      = "isLocked";
        inline constexpr auto btState       = "btState";
        inline constexpr auto btOn          = "on";
        inline constexpr auto btCommand     = "btCommand";
        inline constexpr auto changeSim     = "changeSim";
        inline constexpr auto getInfo       = "getInfo";

        /// values for getInfo cmd
        inline constexpr auto simStateInfo = "simState";
    }
} // namespace parserFSM
