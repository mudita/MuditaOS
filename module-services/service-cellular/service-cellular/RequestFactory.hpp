// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <memory>
#include <map>
#include <functional>

#include <modem/mux/DLCChannel.h>

#include "requests/CallRequest.hpp"
#include "service-appmgr/Actions.hpp"
#include "CellularMessage.hpp"

namespace cellular
{
    using CreateCallback = std::function<std::unique_ptr<IRequest>(const std::string &, GroupMatch)>;

    class RequestFactory
    {
      public:
        RequestFactory(const std::string &data,
                       at::BaseChannel &channel,
                       cellular::api::CallMode callMode,
                       bool simInserted);
        std::unique_ptr<IRequest> create();

      private:
        void registerRequest(std::string regex, CreateCallback);
        std::unique_ptr<IRequest> emergencyCheck();

        std::string request;
        std::vector<std::pair<std::string, CreateCallback>> requestMap;

        at::BaseChannel &channel;
        const cellular::api::CallMode callMode;
        const bool simInserted;
    };
} // namespace cellular
