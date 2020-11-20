// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/Result.hpp>
#include <at/Commands.hpp>
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
        /**
         * Helper command for building output command
         * @param atCommand
         * @param builderFunctions functions that build parts of the output command in order of execution
         * @return formatted command or empty string if input is invalid
         */
        auto buildCommand(at::AT atCommand, const std::vector<std::function<std::string()>> &builderFunctions) const
            -> std::string;
        bool isRequestHandled = false;
        std::string request;
    };
}; // namespace cellular
