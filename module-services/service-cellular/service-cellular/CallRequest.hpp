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
        virtual std::string command(void) = 0;

        virtual void handle(CallRequestHandler &h, at::Result &result) = 0;
        virtual void setHandled(bool handled)                          = 0;
        virtual bool isHandled(void)                                   = 0;
        virtual bool checkModemResponse(const at::Result &result)      = 0;
    };

    class Request : public IRequest
    {
      private:
        bool isRequestHandled = false;

      public:
        void setHandled(bool handled) final
        {
            isRequestHandled = handled;
        }
        bool isHandled(void) final
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
        IMEIRequest() = delete;
        IMEIRequest(const std::string &data){};
        ~IMEIRequest(){};
        std::string command(void) override final;
        static std::unique_ptr<IMEIRequest> create(const std::string &data);
        void handle(CallRequestHandler &h, at::Result &result) final
        {
            h.handle(*this, result);
        }
    };

    class USSDRequest : public Request
    {
      private:
        std::string request;

      public:
        USSDRequest() = delete;
        USSDRequest(const std::string &data) : request(data){};
        ~USSDRequest(){};
        std::string command(void) override final;

        static std::unique_ptr<USSDRequest> create(const std::string &data);
        void handle(CallRequestHandler &h, at::Result &result) final
        {
            h.handle(*this, result);
        }
    };

    class CallRequest : public Request
    {
      private:
        std::string request;

      public:
        CallRequest() = delete;
        CallRequest(const std::string &data) : request(data){};
        ~CallRequest(){};
        std::string command(void) override final;
        std::string getNumber(void)
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
