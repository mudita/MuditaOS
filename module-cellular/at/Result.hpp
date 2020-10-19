// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <variant>
#include <string>
#include <vector>

#include <ErrorCode.hpp>

namespace at
{
    struct Result
    {
        /// result class for AT send -> receive command, could return promise :p
        enum class Code
        {
            OK,      // at OK
            ERROR,   // at ERROR
            TIMEOUT, // at Timeout
            TOKENS,  // at numbers of tokens needed met
            NONE,    // no code
        } code = Code::NONE;


        //! Information about Equipment or Network error as variant type
        /*!
         * Example of checking for specific error type
            if (std::holds_alternative<at::EquipmentErrorCode>(errorCode)){
                std::get<at::EquipmentErrorCode>(errorCode);
            }
        */
        std::variant<at::EquipmentErrorCode, at::NetworkErrorCode> errorCode = at::EquipmentErrorCode::NoInformation;


        std::vector<std::string> response;



        explicit operator bool() const
        {
            return code == Code::OK;
        }
    };
} // namespace at
