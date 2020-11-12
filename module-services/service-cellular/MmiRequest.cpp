// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <memory>

#include <at/Commands.hpp>

#include "service-cellular/MmiRequest.hpp"
#include "service-cellular/MmiRequestCallForwarding.hpp"
#include "service-cellular/CallRequestFactory.hpp"

namespace call_request
{
    MmiRequest::MmiRequest(const std::string &data, GroupMatch matchGroups)
        : Request(data), serviceCode(matchGroups[magic_enum::enum_integer(MMIRegexGroups::ServiceCode)]),
          supplementaryInfoA(matchGroups[magic_enum::enum_integer(MMIRegexGroups::SupInfoA)]),
          supplementaryInfoB(matchGroups[magic_enum::enum_integer(MMIRegexGroups::SupInfoB)]),
          supplementaryInfoC(matchGroups[magic_enum::enum_integer(MMIRegexGroups::SupInfoC)])
    {
        static_assert(magic_enum::enum_count<MMIRegexGroups>() == maxGroupsInMmiRequest);

        std::map<std::string, ProcedureType> procedureTypeMap = {
            {"*", ProcedureType::Activation},
            {"#", ProcedureType::Deactivation},
            {"*#", ProcedureType::Interrogation},
            {"**", ProcedureType::Registration},
            {"##", ProcedureType::Erasure},
        };

        auto &procedureTypeString = matchGroups[magic_enum::enum_integer(MMIRegexGroups::ProcedureType)];
        if (auto pType = procedureTypeMap.find(procedureTypeString); pType != procedureTypeMap.end()) {
            procedureType = pType->second;
            return;
        }

        throw std::runtime_error("Procedure type not covered by constructor.");
    }

    std::unique_ptr<MmiRequest> MmiRequest::create(const std::string &data, GroupMatch matchGroups)
    {
        std::string serviceCode = matchGroups[magic_enum::enum_integer(MMIRegexGroups::ServiceCode)];

        auto factoryList = {MmiRequestCallForwarding::create};

        for (auto &createClbk : factoryList) {
            try {
                if (auto req = createClbk(serviceCode, data, matchGroups)) {
                    return req;
                }
            }
            catch (std::runtime_error &except) {
                LOG_ERROR("Failed to create MMI request. Error message:\n%s", except.what());
            }
        }

        return nullptr;
    }

}; // namespace call_request
