// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/BaseHelper.hpp>
#include <endpoints/Context.hpp>
#include <Service/Common.hpp>
#include <Service/Service.hpp>

namespace sys
{
    class Service;
} // namespace sys

namespace parserFSM
{
    class DeviceInfoEndpointHelper : public BaseHelper
    {
      public:
        explicit DeviceInfoEndpointHelper(sys::Service *p) : BaseHelper(p)
        {}

      private:
        auto processGet(Context &context) -> ProcessResult final;
        bool requestDeviceInfo(sys::Service *serv);
    };
} // namespace parserFSM
