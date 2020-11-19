// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/Result.hpp>
#include "service-cellular/RequestHandler.hpp"

namespace cellular
{
    constexpr inline auto maxGroupsInRequest = 6;
    using GroupMatch                         = std::array<std::reference_wrapper<std::string>, maxGroupsInRequest>;

    class IRequest
    {
      public:
        virtual std::string command()                              = 0;
        virtual void handle(RequestHandler &h, at::Result &result) = 0;
        virtual void setHandled(bool handled)                      = 0;
        virtual bool isHandled() const noexcept                    = 0;
        virtual bool checkModemResponse(const at::Result &result)  = 0;
        virtual bool isValid() const noexcept                      = 0;
        virtual ~IRequest(){};
    };

    class Request : public IRequest
    {
      public:
        Request(const std::string &data) : request(data){};

        void setHandled(bool handled) final;
        bool isHandled() const noexcept final;
        bool checkModemResponse(const at::Result &result) final;
        bool isValid() const noexcept override;

      protected:
        bool isRequestHandled = false;
        std::string request;
    };
}; // namespace cellular
