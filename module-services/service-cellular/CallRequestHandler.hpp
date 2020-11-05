// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
namespace call_request
{
    class IMEIRequest;
    class USSDRequest;
    class CallRequest;

    class CallRequestHandler
    {
      public:
        virtual void handle(IMEIRequest &request) = 0;
        virtual void handle(USSDRequest &request) = 0;
        virtual void handle(CallRequest &request) = 0;
    };

} // namespace call_request
