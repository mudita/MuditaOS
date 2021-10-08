// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/Result.hpp>
#include "service-cellular/requests/CallRequest.hpp"

namespace cellular
{
    constexpr inline auto SupplementaryServicesRegex =
        R"(^(\*|\*\*|\*\#|\#|\#\#)(\d{2,3})(?:\*([^\*\#]*))?(?:\*([^\*\#]*))?(?:\*([^\*\#]*))?\#)";

    /**
     * (according to 3GPP TS 22.030 V16.0.0):
     */
    enum class SupplementaryServicesRegexGroups
    {
        ProcedureType, //! (\*|\*\*|\*\#|\#|\#\#)
        //!    * (activation/registration),
        //!    # (deactivation),
        //!    *# (interrogation),
        //!    ** (registration),
        //!    ## (erasure)
        ServiceCode, //! (\d{2,3})
        SupInfoA,    //! (?:\*([^\*\#]+))  - Supplementary Information A (optional)
        SupInfoB,    //! (?:\*([^\*\#]+))  - Supplementary Information B (optional)
        SupInfoC     //! (?:\*([^\*\#]+))  - Supplementary Information C (optional)
    };

    class SupplementaryServicesRequest : public Request
    {
      public:
        static std::unique_ptr<SupplementaryServicesRequest> create(const std::string &data, GroupMatch matchGroups);
        SupplementaryServicesRequest(const std::string &data, GroupMatch matchGroups);

        // according to 3GPP TS 22.030 V16.0.0
        enum class ProcedureType
        {
            Deactivation,  ///< MMI string starts with #
            Activation,    ///< MMI string starts with *
            Interrogation, ///< MMI string starts with *#
            Registration,  ///< MMI string starts with **
            Erasure        ///< MMI string starts with ##
        };

        auto getProcedureType() -> ProcedureType;

      protected:
        std::string request;
        std::string serviceCode;
        std::string supplementaryInfoA;
        std::string supplementaryInfoB;
        std::string supplementaryInfoC;
        ProcedureType procedureType;

        // according to EC25&EC21_AT_Commands_Manual_V1.3
        static constexpr inline auto atInformationClassVoice     = 1 << 0;
        static constexpr inline auto atInformationClassData      = 1 << 1;
        static constexpr inline auto atInformationClassFax       = 1 << 2;
        static constexpr inline auto atInformationClassSms       = 1 << 3;
        static constexpr inline auto atInformationClassDataSync  = 1 << 4;
        static constexpr inline auto atInformationClassDataAsync = 1 << 5;

        static constexpr inline auto atInformationClassAllTele =
            atInformationClassVoice + atInformationClassFax + atInformationClassSms;
        static constexpr inline auto atInformationClassAllDataTele = atInformationClassFax + atInformationClassSms;
        static constexpr inline auto atInformationClassAllBearer =
            atInformationClassDataSync + atInformationClassDataAsync + atInformationClassData;

        // according to 3GPP TS 22.030 V16.0.0
        enum class TeleAndBearerService
        {
            AllTeleServices          = 10,
            Telephony                = 11,
            AllDataTeleServices      = 12,
            FacsimileServices        = 13,
            ShortMessageServices     = 16,
            AllTeleServicesExceptSms = 19,

            VoiceGroupCallService = 17,
            VoiceBroadcastService = 18,

            AllBearerServices           = 20,
            AllAsyncServices            = 21,
            AllSyncServices             = 22,
            AllDataCircuitSync          = 24,
            AllDataCircuitAsync         = 25,
            AllGprsBearerServices       = 99,
            TelephonyAndAllSyncServices = 26,
        };

        auto getCommandInformationClass(const std::string &basicServiceGroup) const -> std::optional<std::string>;
    };
}; // namespace cellular
