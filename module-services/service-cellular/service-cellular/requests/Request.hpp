// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/Result.hpp>
#include <at/Commands.hpp>
#include "service-cellular/RequestHandler.hpp"
#include <at/Cmd.hpp>

namespace cellular
{
    constexpr inline auto maxGroupsInRequest = 6;
    using GroupMatch                         = std::array<std::reference_wrapper<std::string>, maxGroupsInRequest>;

    class IRequest
    {
      public:
        virtual at::Cmd command()                                  = 0;
        virtual void handle(RequestHandler &h, at::Result &result) = 0;
        virtual void setHandled(bool handled)                      = 0;
        virtual bool checkModemResponse(const at::Result &result)  = 0;
        [[nodiscard]] virtual bool isHandled() const noexcept      = 0;
        [[nodiscard]] virtual bool isValid() const noexcept        = 0;
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
        using commandBuilderFunc = std::function<std::string()>;
        /**
         * Helper command for building output command
         * @param atCommand
         * @param builderFunctions functions that build parts of the output command in order of execution
         * @param trim true to avoid appending commands that evaluate to empty string
         * @return formatted command or empty string if input is invalid
         */
        auto buildCommand(at::AT atCommand,
                          const std::vector<commandBuilderFunc> &builderFunctions,
                          bool trim = true) const -> at::Cmd;
        bool isRequestHandled = false;
        std::string request;
    };
}; // namespace cellular
