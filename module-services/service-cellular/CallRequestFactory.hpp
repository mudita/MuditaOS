// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

namespace call_request
{

    class IRequest
    {

      public:
        bool callRequest                  = false;
        bool ussdRequest                  = false;
        virtual std::string process(void) = 0;
    };

    class IMEIRequest : public IRequest
    {
      public:
        IMEIRequest() = delete;
        IMEIRequest(const std::string &data){};
        ~IMEIRequest(){};
        std::string process(void) override final;
    };

    class USSDRequest : public IRequest
    {
      public:
        USSDRequest() = delete;
        USSDRequest(const std::string &data)
        {
            ussdRequest = true;
        };
        ~USSDRequest(){};
        std::string process(void) override final;
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
    };

    class Factory
    {
      private:
        std::string request;

      public:
        Factory() = delete;
        Factory(const std::string &data) : request(data){};
        IRequest *create(void);

        bool isCallRequest(void);
    };
} // namespace call_request
