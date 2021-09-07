// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SupplementaryServicesRequest.hpp"

namespace cellular
{
    constexpr inline auto PasswordRegistrationRegex =
        R"(^(\*\*|\*)(03)(?:\*(30|31|330|331|33|332|333|35|351|353|))(?:\*([0-9]*))(?:\*([0-9]*))(?:\*([0-9]*))\#$)";

    /**
     * (according to 3GPP TS 22.030 V16.0.0):
     */
    enum class PasswordRegistrationRegexGroups
    {
        ProcedureType, //! (\*\*|\*)
        //!    * (activation/registration),
        //!    ** (registration),
        ServiceCode,        //! (03)
        BarringServiceCode, //! (?:\*(30|31|330|331|33|332|333|35|351|353|))
        OldPassword,        //! (?:\*([0-9]*)
        NewPassword,        //! (?:\*([0-9]*)
        NewPasswordRepeat   //! (?:\*([0-9]*)
    };

    class PasswordRegistrationRequest : public Request
    {
      public:
        PasswordRegistrationRequest(const std::string &data, GroupMatch matchGroups)
            : Request(data),
              requestBarringService(
                  matchGroups[magic_enum::enum_integer(PasswordRegistrationRegexGroups::BarringServiceCode)]),
              requestOldPassword(matchGroups[magic_enum::enum_integer(PasswordRegistrationRegexGroups::OldPassword)]),
              requestNewPassword(matchGroups[magic_enum::enum_integer(PasswordRegistrationRegexGroups::NewPassword)]),
              requestNewPasswordRepeat(
                  matchGroups[magic_enum::enum_integer(PasswordRegistrationRegexGroups::NewPasswordRepeat)])
        {}

        static std::unique_ptr<PasswordRegistrationRequest> create(const std::string &data, GroupMatch matchGroups);

        auto command() -> at::Cmd final;
        void handle(RequestHandler &h, at::Result &result) final;
        auto isValid() const noexcept -> bool final;

        auto getCommandFacility() const noexcept -> std::string;
        auto getOldPassword() const noexcept -> std::string;
        auto getNewPassword() const noexcept -> std::string;

      private:
        std::string requestBarringService;
        std::string requestOldPassword;
        std::string requestNewPassword;
        std::string requestNewPasswordRepeat;
    };
}; // namespace cellular
