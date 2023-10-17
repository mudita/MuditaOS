// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <hal/key_input/KeyEventDefinitions.hpp>
#include <input/InputEvent.hpp>
#include <module-db/Interface/SMSRecord.hpp>
#include <endpoints/BaseHelper.hpp>

namespace sdesktop::endpoints
{

    class DeveloperModeHelper : public BaseHelper
    {
        static auto getKeyCode(int val) noexcept -> bsp::KeyCodes;
        bool sendKeypress(bsp::KeyCodes keyCode, gui::InputEvent::State state);

        void requestSimChange(int simSelected);
        auto smsRecordFromJson(const json11::Json &msgJson) -> SMSRecord;
        bool requestCellularPowerStateChange(int simSelected);
        bool requestServiceStateInfo(sys::Service *serv);
        bool requestCellularSleepModeInfo(sys::Service *serv);
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
        inline constexpr auto ATResponse             = "ATResponse";
        inline constexpr auto AT                     = "AT";
        inline constexpr auto EQ                     = "EQ";
        inline constexpr auto fileName               = "fileName";
        inline constexpr auto fileData               = "fileData";
        inline constexpr auto timeout                = "timeout";
        inline constexpr auto focus                  = "focus";
        inline constexpr auto phoneLocked            = "phoneLocked";
        inline constexpr auto changeSim              = "changeSim";
        inline constexpr auto changeAutoLockTimeout  = "changeAutoLockTimeout";
        inline constexpr auto smsCommand             = "smsCommand";
        inline constexpr auto changeCellularStateCmd = "changeCellularStateCmd";
        inline constexpr auto getInfo                = "getInfo";
        inline constexpr auto tethering              = "tethering";
        inline constexpr auto switchApplication      = "switchApplication";
        inline constexpr auto switchWindow           = "switchWindow";
        inline constexpr auto phoneLockCodeEnabled   = "phoneLockCodeEnabled";

        namespace switchData
        {
            inline constexpr auto applicationName = "applicationName";
            inline constexpr auto windowName      = "windowName";
        } // namespace switchData

        /// values for getInfo cmd
        inline constexpr auto simStateInfo          = "simState";
        inline constexpr auto cellularStateInfo     = "cellularState";
        inline constexpr auto cellularSleepModeInfo = "cellularSleepMode";

        /// values for smsCommand
        inline constexpr auto smsAdd = "smsAdd";

        /// values for tethering
        inline constexpr auto tetheringOn  = "on";
        inline constexpr auto tetheringOff = "off";

    } // namespace json::developerMode

} // namespace sdesktop::endpoints
