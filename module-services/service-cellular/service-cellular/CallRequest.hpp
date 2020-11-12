// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/Result.hpp>
#include "CallRequestHandler.hpp"

namespace call_request
{
    constexpr inline auto maxGroupsInMmiRequest = 5;
    using GroupMatch = std::array<std::reference_wrapper<std::string>, maxGroupsInMmiRequest>;

    class IRequest
    {
      public:
        virtual std::string command()                                  = 0;
        virtual void handle(CallRequestHandler &h, at::Result &result) = 0;
        virtual void setHandled(bool handled)                          = 0;
        virtual bool isHandled() const noexcept                        = 0;
        virtual bool checkModemResponse(const at::Result &result)      = 0;
        virtual ~IRequest(){};
    };

    class Request : public IRequest
    {
      public:
        Request(const std::string &data);
        void setHandled(bool handled) final;
        bool isHandled() const noexcept final;
        bool checkModemResponse(const at::Result &result);

      protected:
        bool isRequestHandled = false;
        std::string request;
    };

    class IMEIRequest : public Request
    {
      public:
        IMEIRequest(const std::string &data) : Request(data){};
        std::string command() final;

        static std::unique_ptr<IMEIRequest> create(const std::string &data, GroupMatch);
        void handle(CallRequestHandler &h, at::Result &result) final;
    };

    class USSDRequest : public Request
    {
      public:
        USSDRequest(const std::string &data) : Request(data){};
        std::string command() final;

        static std::unique_ptr<USSDRequest> create(const std::string &data, GroupMatch);
        void handle(CallRequestHandler &h, at::Result &result) final;
    };

    class CallRequest : public Request
    {
      public:
        CallRequest(const std::string &data) : Request(data){};
        std::string command() final;
        std::string getNumber() const
        {
            return request;
        }
        static std::unique_ptr<CallRequest> create(const std::string &data, GroupMatch);
        void handle(CallRequestHandler &h, at::Result &result) final;
    };
}; // namespace call_request
