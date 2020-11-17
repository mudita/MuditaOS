// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <memory>
#include <map>
#include <functional>

#include "service-cellular/requests/CallRequest.hpp"
#include "service-cellular/requests/SupplementaryServicesRequest.hpp"

namespace call_request
{
    using CreateCallback = std::function<std::unique_ptr<IRequest>(const std::string &, GroupMatch)>;

    class Factory
    {
      public:
        Factory(const std::string &data);
        std::unique_ptr<IRequest> create();

      private:
        std::string request;
        std::map<std::string, CreateCallback> requestMap;
        void registerRequest(std::string regex, CreateCallback);
    };
} // namespace call_request
