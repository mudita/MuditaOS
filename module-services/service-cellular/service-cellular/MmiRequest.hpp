// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/Result.hpp>
#include "CallRequest.hpp"

namespace call_request
{
    constexpr auto MMIRegex =
        "^(\\*|\\*\\*|\\*\\#|\\#|\\#\\#)(\\d{2,3})(?:\\*([^\\*\\#]+))?(?:\\*([^\\*\\#]+))?(?:\\*([^\\*\\#]+))?\\#";

    /**
     * (according to 3GPP TS 22.030 V16.0.0):
     */
    enum class MMIRegexGroups
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

    class MmiRequest : public Request
    {
      private:
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

      public:
        static std::unique_ptr<MmiRequest> create(const std::string &data, GroupMatch matchGroups);
        MmiRequest(const std::string &data, GroupMatch matchGroups);
    };
}; // namespace call_request
