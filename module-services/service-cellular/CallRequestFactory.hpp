// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <memory>
#include <map>
#include <functional>

namespace call_request
{

    class IRequest
    {

      public:
        bool callRequest                  = false;
        bool ussdRequest                  = false;
        virtual std::string process(void) = 0;
    };

    constexpr auto IMEIRegex = "(\\*#06#)";

    class IMEIRequest : public IRequest
    {
      public:
        IMEIRequest() = delete;
        IMEIRequest(const std::string &data){};
        ~IMEIRequest(){};
        std::string process(void) override final;
        static std::unique_ptr<IMEIRequest> create(const std::string &data);
    };

    class USSDRequest : public IRequest
    {
      private:
        std::string requestData;

      public:
        USSDRequest() = delete;
        USSDRequest(const std::string &data) : requestData(data)
        {
            ussdRequest = true;
        };
        ~USSDRequest(){};
        std::string process(void) override final;

        static std::unique_ptr<USSDRequest> create(const std::string &data);
    };

    class CallRequest : public IRequest
    {
      private:
        std::string request;

      public:
        CallRequest() = delete;
        CallRequest(const std::string &data) : request(data)
        {
            callRequest = true;
        };
        ~CallRequest(){};
        std::string process(void) override final;

        static std::unique_ptr<CallRequest> create(const std::string &data);
    };

    class Factory
    {
      private:
        std::string request;
        std::map<std::string, std::function<std::unique_ptr<IRequest>(const std::string &)>> requestMap;
        void registerRequest(std::string regex, std::function<std::unique_ptr<IRequest>(const std::string &)>);

      public:
        Factory() = delete;
        Factory(const std::string &data) : request(data)
        {
            registerRequest(IMEIRegex, IMEIRequest::create);
        };
        std::unique_ptr<IRequest> create(void);

        bool isCallRequest(void);
    };
} // namespace call_request
