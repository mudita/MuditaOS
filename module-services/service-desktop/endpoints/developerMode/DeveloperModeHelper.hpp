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
        auto processPutKeyPressed(Context &context) -> ProcessResult;
        auto processPutAT(Context &context) -> ProcessResult;
        auto processPutFocus() -> ProcessResult;
        auto processPutIsLocked() -> ProcessResult;
        auto processPutChangeAutoLockTimeout(Context &context) -> ProcessResult;
        auto processPutChangeSim(Context &context) -> ProcessResult;
        auto processPutChangeCellularState(Context &context) -> ProcessResult;
        auto processPutSms(Context &context) -> ProcessResult;
        auto processPutTethering(Context &context) -> ProcessResult;
        auto processPutUsbSecurity(Context &context) -> ProcessResult;
        auto processPutAppMgr(Context &context) -> ProcessResult;

        auto processGet(Context &context) -> ProcessResult final;
        auto processGetInfo(Context &context) -> ProcessResult;
        auto processGetAppMgr(Context &context) -> ProcessResult;
    };

    namespace json::developerMode
    {
        namespace cmd
        {
            inline constexpr auto keyPressed            = "keyPressed";
            inline constexpr auto state                 = "state";
            inline constexpr auto systemStarted         = "systemStarted";
            inline constexpr auto ATResponse            = "ATResponse";
            inline constexpr auto AT                    = "AT";
            inline constexpr auto focus                 = "focus";
            inline constexpr auto isLocked              = "isLocked";
            inline constexpr auto changeSim             = "changeSim";
            inline constexpr auto sms                   = "smsCommand";
            inline constexpr auto changeAutoLockTimeout = "changeAutoLockTimeout";
            inline constexpr auto changeCellularState   = "changeCellularStateCmd";
            inline constexpr auto getInfo               = "getInfo";
            inline constexpr auto tethering             = "tethering";
            inline constexpr auto usbSecurityStatus     = "usbSecurityStatus";
            inline constexpr auto appmgr                = "applicationManager";
        } // namespace cmd

        namespace at
        {
            inline constexpr auto timeout = "timeout";
        } // namespace at

        namespace getInfo
        {
            inline constexpr auto simStateInfo      = "simState";
            inline constexpr auto cellularStateInfo = "cellularState";
        } // namespace getInfo

        namespace sms
        {
            inline constexpr auto smsAdd = "smsAdd";
        } // namespace sms

        namespace tethering
        {
            inline constexpr auto on  = "on";
            inline constexpr auto off = "off";
        } // namespace tethering

        namespace usbSecurityStatus
        {
            inline constexpr auto locked   = "locked";
            inline constexpr auto unlocked = "unlocked";
        } // namespace usbSecurityStatus

        namespace appmgr
        {
            inline constexpr auto appStack   = "applicationStack";
            inline constexpr auto startApp   = "startApplication";
            inline constexpr auto closeApp   = "closeApplication";
            inline constexpr auto switchBack = "switchBack";
        } // namespace appmgr
    } // namespace json::developerMode

} // namespace parserFSM
