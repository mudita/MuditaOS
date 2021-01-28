// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <memory>
#include <map>
#include <functional>

#include <Modem/TS0710/DLC_channel.h>

#include "requests/CallRequest.hpp"
#include "service-appmgr/Actions.hpp"
#include "CellularMessage.hpp"

namespace cellular
{
    using CreateCallback = std::function<std::unique_ptr<IRequest>(const std::string &, GroupMatch)>;

    class RequestFactory
    {
      public:
        enum class SimStatus
        {
            SimInsterted,
            SimSlotEmpty
        };

        RequestFactory(const std::string &data,
                       at::BaseChannel &channel,
                       CellularCallRequestMessage::RequestMode requestMode,
                       SimStatus simCardStatus);
        std::unique_ptr<IRequest> create();
        std::optional<app::manager::actions::Action> &getActionRequest();

      private:
        void registerRequest(std::string regex, CreateCallback);
        std::unique_ptr<IRequest> emergencyCheck();

        std::string request;
        std::vector<std::pair<std::string, CreateCallback>> requestMap;
        std::optional<app::manager::actions::Action> actionRequest;

        at::BaseChannel &channel;
        const CellularCallRequestMessage::RequestMode requestMode;
        const SimStatus simStatus;
    };
} // namespace cellular
