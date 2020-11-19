// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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

        enum class ProcedureType
        {
            Deactivation,
            Activation,
            Interrogation,
            Registration,
            Erasure
        };

      protected:
        std::string request;
        std::string serviceCode;
        std::string supplementaryInfoA;
        std::string supplementaryInfoB;
        std::string supplementaryInfoC;
        ProcedureType procedureType;

        static constexpr inline auto atInformationClassVoice     = "1";
        static constexpr inline auto atInformationClassData      = "2";
        static constexpr inline auto atInformationClassFax       = "4";
        static constexpr inline auto atInformationClassAllButSms = "7";

        static constexpr inline auto basicServiceVoice     = "11";
        static constexpr inline auto basicServiceVoiceData = "25";
        static constexpr inline auto basicServiceVoiceFax  = "13";

        auto getCommandInformationClass(const std::string &basicServiceGroup) const -> std::string;
    };
}; // namespace cellular
