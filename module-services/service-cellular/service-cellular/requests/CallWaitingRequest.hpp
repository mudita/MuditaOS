// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SupplementaryServicesRequest.hpp"

namespace cellular
{
    class CallWaitingRequest : public SupplementaryServicesRequest
    {
      public:
        CallWaitingRequest(const std::string &data, GroupMatch matchGroups)
            : SupplementaryServicesRequest(data, matchGroups)
        {}

        static std::unique_ptr<SupplementaryServicesRequest> create(const std::string &serviceCode,
                                                                    const std::string &data,
                                                                    GroupMatch matchGroups);

        auto command() -> at::Cmd final;
        void handle(RequestHandler &h, at::Result &result) final;
        auto isValid() const noexcept -> bool final;

        auto getCommandPresentation() const noexcept -> std::string;
        auto getCommandMode() const noexcept -> std::string;
        auto getCommandClass() const noexcept -> std::string;

      private:
        static constexpr auto callWaitingServiceCode      = "43";
        static constexpr auto unsolicitedResultCodeEnable = "1";

        std::string &basicServiceGroup = supplementaryInfoA;
    };
}; // namespace cellular
