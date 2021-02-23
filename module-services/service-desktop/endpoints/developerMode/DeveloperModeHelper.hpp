// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Context.hpp>

#include <Common/Query.hpp>
#include <Service/Common.hpp>
#include <Service/Service.hpp>
#include <bsp/keyboard/key_codes.hpp>
#include <input/InputEvent.hpp>
#include <module-db/Interface/SMSRecord.hpp>
#include "Mode/BaseHelper.hpp"

namespace sys
{
    class Service;
} // namespace sys

namespace parserFSM
{

    class DeveloperModeHelper : public BaseHelper
    {
        static auto getKeyCode(int val) noexcept -> bsp::KeyCodes;
        bool sendKeypress(bsp::KeyCodes keyCode, gui::InputEvent::State state);

        void requestSimChange(const int simSelected);
        auto smsRecordFromJson(json11::Json msgJson) -> SMSRecord;
        bool requestCellularPowerStateChange(const int simSelected);
        bool requestServiceStateInfo(sys::Service *serv);
        auto prepareSMS(Context &context) -> ProcessResult;

      public:
        explicit DeveloperModeHelper(sys::Service *p) : BaseHelper(p)
        {}

      private:
        auto processPut(Context &context) -> ProcessResult final;
        auto processGet(Context &context) -> ProcessResult final;
    };

    namespace json::developerMode
    {
        inline constexpr auto keyPressed             = "keyPressed";
        inline constexpr auto state                  = "state";
        inline constexpr auto systemStarted          = "systemStarted";
        inline constexpr auto ATResponse             = "ATResponse";
        inline constexpr auto AT                     = "AT";
        inline constexpr auto focus                  = "focus";
        inline constexpr auto isLocked               = "isLocked";
        inline constexpr auto changeSim              = "changeSim";
        inline constexpr auto smsCommand             = "smsCommand";
        inline constexpr auto changeCellularStateCmd = "changeCellularStateCmd";
        inline constexpr auto getInfo                = "getInfo";

        /// values for getInfo cmd
        inline constexpr auto simStateInfo      = "simState";
        inline constexpr auto cellularStateInfo = "cellularState";

        /// values for smsCommand
        inline constexpr auto smsAdd = "smsAdd";
    }
} // namespace parserFSM
