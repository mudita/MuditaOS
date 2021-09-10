// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <endpoints/Context.hpp>
#include <Service/Common.hpp>
#include <Service/Service.hpp>

namespace sdesktop::endpoints
{

    class BluetoothHelper
    {
        sys::Service *ownerServicePtr = nullptr;
        void sendRequest(Context &context, std::unique_ptr<sys::Message> msg);

      public:
        BluetoothHelper(sys::Service *_ownerServicePtr) : ownerServicePtr(_ownerServicePtr){};
        auto processPutRequest(Context &context) -> sys::ReturnCodes;
        auto processGetRequest(Context &context) -> sys::ReturnCodes;
        auto processPostRequest(Context &context) -> sys::ReturnCodes;
        auto processDeleteRequest(Context &context) -> sys::ReturnCodes;
    };

    namespace json::bluetooth
    {
        inline constexpr auto state = "state";
        namespace states
        {
            inline constexpr auto power      = "power";
            inline constexpr auto visibility = "visibility";
        } // namespace states
        inline constexpr auto on   = "on";
        inline constexpr auto off  = "off";
        inline constexpr auto scan = "scan";

        inline constexpr auto command = "command";
        namespace commands
        {
            inline constexpr auto scanOn           = "scanOn";
            inline constexpr auto scanOff          = "scanOff";
            inline constexpr auto disconnect       = "disconnect";
            inline constexpr auto changeVisibility = "changeVisibility";
        } // namespace commands
        inline constexpr auto success = "success";
        inline constexpr auto failure = "failure";

        inline constexpr auto pair    = "pair";
        inline constexpr auto unpair  = "unpair";
        inline constexpr auto connect = "connect";
        inline constexpr auto devices = "devices";
        namespace devicesValues
        {
            inline constexpr auto address = "address";
            inline constexpr auto name    = "name";
            inline constexpr auto scanned = "scanned";
            inline constexpr auto bonded  = "bonded";
        } // namespace devicesValues
    }     // namespace json::bluetooth

} // namespace sdesktop::endpoints
