// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SupplementaryServicesRequest.hpp"

namespace cellular
{
    class CallForwardingRequest : public SupplementaryServicesRequest
    {
      public:
        CallForwardingRequest(const std::string &forwardReason, const std::string &data, GroupMatch matchGroups)
            : SupplementaryServicesRequest(data, matchGroups), forwardReason(forwardReason)
        {}

        static std::unique_ptr<SupplementaryServicesRequest> create(const std::string &serviceCode,
                                                                    const std::string &data,
                                                                    GroupMatch matchGroups);

        auto command() -> at::Cmd final;
        void handle(RequestHandler &h, at::Result &result) final;
        auto isValid() const noexcept -> bool final;

      private:
        // source: EC25&EC21_AT_Commands_Manual
        static constexpr auto addressFormatTypeInternational = "145";
        static constexpr auto addressFormatTypeDefault       = "129";
        static constexpr auto subaddrDefault                 = "";
        static constexpr auto maxNoReplyTime                 = 30;
        static constexpr auto minNoReplyTime                 = 0;

        std::string forwardReason;
        std::string &phoneNumber           = supplementaryInfoA;
        std::string &basicServiceGroup     = supplementaryInfoB;
        std::string &noReplyConditionTimer = supplementaryInfoC;

        // command decomposition according to EC25&EC21_AT_Commands_Manual_V1.3
        auto getCommandReason() const -> std::string;
        auto getCommandMode() const -> std::string;
        auto getCommandNumber() const -> std::string;
        auto getCommandType() const -> std::string;
        auto getCommandClass() const -> std::string;
        auto getCommandSubAddr() const -> std::string;
        auto getCommandSatype() const -> std::string;
        auto getCommandTime() const -> std::string;
    };
}; // namespace cellular
