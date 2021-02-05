// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <endpoints/Context.hpp>

#include <Common/Query.hpp>
#include <Service/Common.hpp>
#include <Service/Service.hpp>

namespace sys
{
    class Service;
} // namespace sys

namespace parserFSM
{

    class BluetoothHelper
    {
        sys::Service *ownerServicePtr = nullptr;

      public:
        BluetoothHelper(sys::Service *_ownerServicePtr) : ownerServicePtr(_ownerServicePtr){};
        auto processPutRequest(Context &context) -> sys::ReturnCodes;
        auto processGetRequest(Context &context) -> sys::ReturnCodes;
    };

    namespace json::bluetooth
    {
        inline constexpr auto state   = "state";
        inline constexpr auto btOn    = "on";
        inline constexpr auto btOff   = "off";
        inline constexpr auto scanOn  = "scanOn";
        inline constexpr auto scanOff = "scanOff";
        inline constexpr auto scan    = "scan";
        inline constexpr auto command = "command";
    } // namespace json::bluetooth
} // namespace parserFSM
