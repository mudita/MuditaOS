// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SupplementaryServicesRequest.hpp"

namespace
{
    // according to EC25&EC21_AT_Commands_Manual_V1.3
    const std::map<std::string, std::string> barringServiceToFacility = {{
        {"33", "AO"},  // BAOC (Bar All Outgoing Calls)
        {"331", "OI"}, // BOIC (Bar Outgoing International Calls)
        {"332", "OX"}, // BOIC-exHC (Bar Outgoing International Calls except to home country)
        {"35", "AI"},  // BAIC (Bar All Incoming Calls)
        {"351", "IR"}, // BIC-Roam (Bar Incoming Calls when Roaming outside the home country)
        {"330", "AB"}, // All barring services
        {"333", "AG"}, // All outgoing barring services
        {"353", "AC"}, // All incoming barring services
        {"", "AB"},    // All incoming barring services (According to 3GPP TS 22.030 V16.0.0 )
    }};
} // namespace

namespace cellular
{
    class CallBarringRequest : public SupplementaryServicesRequest
    {
      public:
        CallBarringRequest(const std::string &facility, const std::string &data, GroupMatch matchGroups)
            : SupplementaryServicesRequest(data, matchGroups), facility(facility)
        {}

        static std::unique_ptr<SupplementaryServicesRequest> create(const std::string &serviceCode,
                                                                    const std::string &data,
                                                                    GroupMatch matchGroups);

        auto command() -> std::string final;
        void handle(RequestHandler &h, at::Result &result) final;
        auto isValid() const noexcept -> bool;

      private:
        std::string facility;
        std::string &password          = supplementaryInfoA;
        std::string &basicServiceGroup = supplementaryInfoB;

        // command decomposition according to EC25&EC21_AT_Commands_Manual_V1.3
        auto getCommandFacility() const -> std::string;
        auto getCommandMode() const -> std::string;
        auto getCommandPassword() const -> std::string;
        auto getCommandClass() const -> std::string;
    };
}; // namespace cellular
