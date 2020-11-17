// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <memory>

#include <at/Commands.hpp>

#include "service-cellular/requests/SupplementaryServicesRequest.hpp"
#include "service-cellular/requests/CallForwardingRequest.hpp"

namespace
{
    const std::map<std::string, call_request::SupplementaryServicesRequest::ProcedureType> procedureTypeMap = {
        {"*", call_request::SupplementaryServicesRequest::ProcedureType::Activation},
        {"#", call_request::SupplementaryServicesRequest::ProcedureType::Deactivation},
        {"*#", call_request::SupplementaryServicesRequest::ProcedureType::Interrogation},
        {"**", call_request::SupplementaryServicesRequest::ProcedureType::Registration},
        {"##", call_request::SupplementaryServicesRequest::ProcedureType::Erasure},
    };
}

namespace call_request
{
    SupplementaryServicesRequest::SupplementaryServicesRequest(const std::string &data, GroupMatch matchGroups)
        : Request(data),
          serviceCode(matchGroups[magic_enum::enum_integer(SupplementaryServicesRegexGroups::ServiceCode)]),
          supplementaryInfoA(matchGroups[magic_enum::enum_integer(SupplementaryServicesRegexGroups::SupInfoA)]),
          supplementaryInfoB(matchGroups[magic_enum::enum_integer(SupplementaryServicesRegexGroups::SupInfoB)]),
          supplementaryInfoC(matchGroups[magic_enum::enum_integer(SupplementaryServicesRegexGroups::SupInfoC)])
    {
        static_assert(magic_enum::enum_count<SupplementaryServicesRegexGroups>() == maxGroupsInMmiRequest);

        auto &procedureTypeString =
            matchGroups[magic_enum::enum_integer(SupplementaryServicesRegexGroups::ProcedureType)];

        auto pType = procedureTypeMap.find(procedureTypeString);
        if (pType == procedureTypeMap.end()) {

            throw std::runtime_error("Procedure type not covered by constructor.");
        }
        procedureType = pType->second;
    }

    auto SupplementaryServicesRequest::create(const std::string &data, GroupMatch matchGroups)
        -> std::unique_ptr<SupplementaryServicesRequest>
    {
        std::string serviceCode = matchGroups[magic_enum::enum_integer(SupplementaryServicesRegexGroups::ServiceCode)];

        auto factoryList = {CallForwardingRequest::create};

        for (auto &createCb : factoryList) {
            try {
                if (auto req = createCb(serviceCode, data, matchGroups)) {
                    return req;
                }
            }
            catch (const std::runtime_error &except) {
                LOG_ERROR("Failed to create MMI request. Error message:\n%s", except.what());
            }
        }

        return nullptr;
    }

    auto SupplementaryServicesRequest::getCommandInformationClass(const std::string &basicServiceGroup) const
        -> std::string
    {
        // according to EC25&EC21_AT_Commands_Manual_V1.3
        if (basicServiceGroup == basicServiceVoice) {
            return atInformationClassVoice;
        }
        else if (basicServiceGroup == basicServiceVoiceFax) {
            return atInformationClassFax;
        }
        else if (basicServiceGroup == basicServiceVoiceData) {
            return atInformationClassData;
        }
        return atInformationClassAllButSms;
    }

}; // namespace call_request
