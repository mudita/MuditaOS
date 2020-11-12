// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-cellular/MmiRequest.hpp"

namespace call_request
{
    class MmiRequestCallForwarding : public MmiRequest
    {
      public:
        MmiRequestCallForwarding(const std::string &forwardReason, const std::string &data, GroupMatch matchGroups)
            : MmiRequest(data, matchGroups), forwardReason(forwardReason)
        {}

        static std::unique_ptr<MmiRequestCallForwarding> create(const std::string &serviceCode,
                                                                const std::string &data,
                                                                GroupMatch matchGroups);

        auto command() -> std::string final;
        void handle(CallRequestHandler &h, at::Result &result) final;

      private:
        std::string forwardReason;
        std::string &phoneNumber           = supplementaryInfoA;
        std::string &basicSericeGroup      = supplementaryInfoB;
        std::string &noReplyConditionTimer = supplementaryInfoB;

        // command decomposition according to EC25&EC21_AT_Commands_Manual_V1.3
        auto getCommandReason() -> std::string;
        auto getCommandMode() -> std::string;
        auto getCommandNumber() -> std::string;
        auto getCommandType() -> std::string;
        auto getCommandClass() -> std::string;
        auto getCommandSubAddr() -> std::string;
        auto getCommandSatype() -> std::string;
        auto getCommandTime() -> std::string;
    };
}; // namespace call_request
