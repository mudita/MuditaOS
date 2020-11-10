// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <memory>
#include <map>
#include <functional>

#include "CallRequest.hpp"

namespace call_request
{

    constexpr auto IMEIRegex = "(\\*#06#)";
    constexpr auto USSDRegex = "^[\\*].*[\\#]$";

    using CreateCallback = std::function<std::unique_ptr<IRequest>(const std::string &)>;

    class Factory
    {
      private:
        std::string request;
        std::map<std::string, std::function<std::unique_ptr<IRequest>(const std::string &)>> requestMap;
        void registerRequest(std::string regex, CreateCallback);

      public:
        Factory() = delete;
        Factory(const std::string &data) : request(data)
        {
            registerRequest(IMEIRegex, IMEIRequest::create);

            /*It have to be last check due to 3GPP TS 22.030*/
            registerRequest(USSDRegex, USSDRequest::create);
        };

        std::unique_ptr<IRequest> create(void);
    };
} // namespace call_request
