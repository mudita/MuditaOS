// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/Result.hpp>

namespace call_request
{
    class IMEIRequest;
    class USSDRequest;
    class CallRequest;
    class SupplementaryServicesRequest;

    class CallRequestHandler
    {
      public:
        virtual void handle(IMEIRequest &request, at::Result &result) = 0;
        virtual void handle(USSDRequest &request, at::Result &result) = 0;
        virtual void handle(CallRequest &request, at::Result &result) = 0;
        virtual void handle(SupplementaryServicesRequest &request, at::Result &result) = 0;
    };
} // namespace call_request
