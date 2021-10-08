// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/Result.hpp>

namespace cellular
{
    class ImeiRequest;
    class UssdRequest;
    class CallRequest;
    class RejectRequest;
    class SupplementaryServicesRequest;
    class PasswordRegistrationRequest;
    class PinChangeRequest;
    class ClipRequest;
    class ClirRequest;
    class CallForwardingRequest;
    class CallBarringRequest;
    class CallWaitingRequest;

    class RequestHandler
    {
      public:
        virtual void handle(ImeiRequest &request, at::Result &result)                  = 0;
        virtual void handle(UssdRequest &request, at::Result &result)                  = 0;
        virtual void handle(CallRequest &request, at::Result &result)                  = 0;
        virtual void handle(RejectRequest &request, at::Result &result)                = 0;
        virtual void handle(PasswordRegistrationRequest &request, at::Result &result)  = 0;
        virtual void handle(PinChangeRequest &request, at::Result &result)             = 0;
        virtual void handle(SupplementaryServicesRequest &request, at::Result &result) = 0;
        virtual void handle(ClirRequest &request, at::Result &result)                  = 0;
        virtual void handle(ClipRequest &request, at::Result &result)                  = 0;
        virtual void handle(CallForwardingRequest &request, at::Result &result)        = 0;
        virtual void handle(CallBarringRequest &request, at::Result &result)           = 0;
        virtual void handle(CallWaitingRequest &request, at::Result &result)           = 0;
    };
} // namespace cellular
