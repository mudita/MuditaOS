// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/Result.hpp>
#include <at/Cmd.hpp>

#include "service-cellular/RequestHandler.hpp"
#include "service-cellular/requests/Request.hpp"

namespace cellular
{
    class RejectRequest : public Request
    {
      public:
        enum class RejectReason
        {
            NoSim,
            NotAnEmergencyNumber,
            NoNetworkConnection
        };

        RejectRequest(RejectReason rejectReason, const std::string &number)
            : Request(number), rejectReason(rejectReason){};

        at::Cmd command() final
        {
            constexpr std::chrono::milliseconds immediateTimeout{0};
            return at::Cmd(std::string(), immediateTimeout);
        }

        void handle(RequestHandler &h, at::Result &result)
        {
            h.handle(*this, result);
        }

        RejectReason getRejectReason()
        {
            return rejectReason;
        }

        std::string getNumber() const
        {
            return request;
        }

      private:
        RejectReason rejectReason;
    };
}; // namespace cellular
