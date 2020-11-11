// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/Result.hpp>
#include "CallRequestHandler.hpp"

namespace call_request
{
    class IRequest
    {
      public:
        virtual std::string command()                                  = 0;
        virtual void handle(CallRequestHandler &h, at::Result &result) = 0;
        virtual void setHandled(bool handled)                          = 0;
        virtual bool isHandled()                                       = 0;
        virtual bool checkModemResponse(const at::Result &result)      = 0;
    };

    class Request : public IRequest
    {
      protected:
        bool isRequestHandled = false;
        std::string request;

      public:
        Request(const std::string &data) : request(data){};
        void setHandled(bool handled) final
        {
            isRequestHandled = handled;
        }
        bool isHandled() final
        {
            return isRequestHandled;
        }
        bool checkModemResponse(const at::Result &result)
        {
            if (result.code != at::Result::Code::OK) {
                return false;
            }
            return true;
        }
    };

    class IMEIRequest : public Request
    {
      public:
        IMEIRequest(const std::string &data) : Request(data){};
        std::string command() final;
        static std::unique_ptr<IMEIRequest> create(const std::string &data);
        void handle(CallRequestHandler &h, at::Result &result) final
        {
            h.handle(*this, result);
        }
    };

    class USSDRequest : public Request
    {
      public:
        USSDRequest(const std::string &data) : Request(data){};
        std::string command() final;

        static std::unique_ptr<USSDRequest> create(const std::string &data);
        void handle(CallRequestHandler &h, at::Result &result) final
        {
            h.handle(*this, result);
        }
    };

    class CallRequest : public Request
    {
      public:
        CallRequest(const std::string &data) : Request(data){};
        std::string command() final;
        std::string getNumber()
        {
            return request;
        };
        static std::unique_ptr<CallRequest> create(const std::string &data);
        void handle(CallRequestHandler &h, at::Result &result) final
        {
            h.handle(*this, result);
        }
    };
}; // namespace call_request
