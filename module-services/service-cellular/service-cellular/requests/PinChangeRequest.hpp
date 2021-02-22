// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SupplementaryServicesRequest.hpp"

namespace cellular
{
    constexpr inline auto PinChangeRegex = R"(^(\*\*)(04|042|05)(?:\*([0-9]*))(?:\*([0-9]*))(?:\*([0-9]*))\#$)";

    // (according to 3GPP TS 22.030 V16.0.0):
    enum class PinChangeRegexGroups
    {
        ProcedureType, //! (\*\*)
        //!    ** (registration),
        ServiceCode,      //! (04|042|05)
        OldPassword,      //! (?:\*([0-9]*))
        NewPassword,      //! (?:\*([0-9]*))
        NewPasswordRepeat //! (?:\*([0-9]*))
    };

    class PinChangeRequest : public Request
    {
      public:
        enum class PassChangeType
        {
            ChangePin,
            ChangePin2,
            ChangePinByPuk,
        };

        PinChangeRequest(const std::string &data, GroupMatch matchGroups);

        static std::unique_ptr<PinChangeRequest> create(const std::string &data, GroupMatch matchGroups);

        auto command() -> at::Cmd final;
        void handle(RequestHandler &h, at::Result &result) final;
        auto isValid() const noexcept -> bool final;

        auto getOldPinOrPuk() const noexcept -> std::string;
        auto getNewPin() const noexcept -> std::string;

      private:
        PassChangeType passChangeType;
        std::string requestOldPinOrPuk;
        std::string requestNewPin;
        std::string requestNewPinRepeat;
    };
}; // namespace cellular
