// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <memory>

#include <at/Commands.hpp>
#include <Utils.hpp>
#include <at/ATFactory.hpp>

#include "service-cellular/requests/SupplementaryServicesRequest.hpp"
#include "service-cellular/requests/CallForwardingRequest.hpp"
#include "service-cellular/requests/ClipRequest.hpp"
#include "service-cellular/requests/ClirRequest.hpp"
#include "service-cellular/requests/ColpRequest.hpp"
#include "service-cellular/requests/CallWaitingRequest.hpp"
#include "service-cellular/requests/CallBarringRequest.hpp"
#include <map>

namespace
{
    const std::map<std::string, cellular::SupplementaryServicesRequest::ProcedureType> procedureTypeMap = {
        {"*", cellular::SupplementaryServicesRequest::ProcedureType::Activation},
        {"#", cellular::SupplementaryServicesRequest::ProcedureType::Deactivation},
        {"*#", cellular::SupplementaryServicesRequest::ProcedureType::Interrogation},
        {"**", cellular::SupplementaryServicesRequest::ProcedureType::Registration},
        {"##", cellular::SupplementaryServicesRequest::ProcedureType::Erasure},
    };
}

namespace cellular
{
    SupplementaryServicesRequest::SupplementaryServicesRequest(const std::string &data, GroupMatch matchGroups)
        : Request(data),
          serviceCode(matchGroups[magic_enum::enum_integer(SupplementaryServicesRegexGroups::ServiceCode)]),
          supplementaryInfoA(matchGroups[magic_enum::enum_integer(SupplementaryServicesRegexGroups::SupInfoA)]),
          supplementaryInfoB(matchGroups[magic_enum::enum_integer(SupplementaryServicesRegexGroups::SupInfoB)]),
          supplementaryInfoC(matchGroups[magic_enum::enum_integer(SupplementaryServicesRegexGroups::SupInfoC)])
    {
        static_assert(magic_enum::enum_count<SupplementaryServicesRegexGroups>() <= maxGroupsInRequest);

        auto &procedureTypeString =
            matchGroups[magic_enum::enum_integer(SupplementaryServicesRegexGroups::ProcedureType)];

        auto pType = procedureTypeMap.find(procedureTypeString);
        if (pType == procedureTypeMap.end()) {

            throw std::runtime_error(std::string(__FUNCTION__) + ": procedure type not covered by constructor.");
        }
        procedureType = pType->second;
    }

    auto SupplementaryServicesRequest::create(const std::string &data, GroupMatch matchGroups)
        -> std::unique_ptr<SupplementaryServicesRequest>
    {
        std::string serviceCode = matchGroups[magic_enum::enum_integer(SupplementaryServicesRegexGroups::ServiceCode)];

        auto factoryList = {
            CallForwardingRequest::create,
            CallWaitingRequest::create,
            CallBarringRequest::create,
            ClipRequest::create,
            ClirRequest::create,
            ColpRequest::create,
        };

        for (auto &createCb : factoryList) {
            if (auto req = createCb(serviceCode, data, matchGroups)) {
                return req;
            }
        }

        return nullptr;
    }

    auto SupplementaryServicesRequest::getCommandInformationClass(const std::string &basicServiceGroup) const
        -> std::optional<std::string>
    {
        int basicGroup       = 0;
        int informationClass = 0;

        if (basicServiceGroup.empty()) {
            // according to 3GPP TS 22.030 V16.0.0 Annex C
            informationClass = atInformationClassAllTele + atInformationClassAllBearer;
        }
        else {
            basicGroup   = utils::getNumericValue<int>(basicServiceGroup);
            auto service = magic_enum::enum_cast<TeleAndBearerService>(basicGroup);

            if (!service.has_value()) {
                return std::nullopt;
            }

            switch (service.value()) {
            // according to 3GPP TS 22.030 V16.0.0 Annex C
            case TeleAndBearerService::AllTeleServices:
                informationClass = atInformationClassAllTele;
                break;
            case TeleAndBearerService::Telephony:
                informationClass = atInformationClassVoice;
                break;
            case TeleAndBearerService::AllDataTeleServices:
                informationClass = atInformationClassAllDataTele;
                break;
            case TeleAndBearerService::FacsimileServices:
                informationClass = atInformationClassFax;
                break;
            case TeleAndBearerService::ShortMessageServices:
                informationClass = atInformationClassSms;
                break;
            case TeleAndBearerService::AllTeleServicesExceptSms:
                informationClass = atInformationClassAllTele - atInformationClassSms;
                break;
            case TeleAndBearerService::VoiceGroupCallService:
                LOG_INFO("Unsupported information class: 17 - Voice Group Call Service");
                break;
            case TeleAndBearerService::VoiceBroadcastService:
                LOG_INFO("Unsupported information class: 18 - Voice Broadcast Service");
                break;
            case TeleAndBearerService::AllBearerServices:
                informationClass = atInformationClassAllBearer;
                break;
            case TeleAndBearerService::AllAsyncServices:
                informationClass = atInformationClassDataAsync;
                break;
            case TeleAndBearerService::AllSyncServices:
                informationClass = atInformationClassDataSync;
                break;
            case TeleAndBearerService::AllDataCircuitSync:
                informationClass = atInformationClassDataSync;
                break;
            case TeleAndBearerService::AllDataCircuitAsync:
                informationClass = atInformationClassDataAsync;
                break;
            case TeleAndBearerService::AllGprsBearerServices:
                LOG_INFO("Unsupported information class: 99 - All Gprs Bearer Services");
                break;
            case TeleAndBearerService::TelephonyAndAllSyncServices:
                informationClass = atInformationClassVoice + atInformationClassDataSync;
                break;
            }
        }

        if (informationClass == 0) {
            return std::nullopt;
        }

        return utils::to_string(informationClass);
    }

    auto SupplementaryServicesRequest::getProcedureType() -> ProcedureType
    {
        return procedureType;
    }

}; // namespace cellular
