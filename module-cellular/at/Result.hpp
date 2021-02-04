// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
            OK,            /// at OK
            ERROR,         /// at ERROR
            TIMEOUT,       /// at Timeout
            TOKENS,        /// at numbers of tokens needed met
            NONE,          /// no code
            UNDEFINED,     /// undefined result - usage of Undefined result, define and pin result to use it
            PARSING_ERROR, /// parser error
        } code = Code::UNDEFINED;

        Result() = default;

        Result(Code code, std::vector<std::string> response) : code(code), response(std::move(response))
        {}

        //! Information about Equipment or Network error as variant type
        /*!
         * Example of checking for specific error type
            if (std::holds_alternative<at::EquipmentErrorCode>(errorCode)){
                std::get<at::EquipmentErrorCode>(errorCode);
            }
        */
        std::variant<at::EquipmentErrorCode, at::NetworkErrorCode> errorCode = at::EquipmentErrorCode::NoInformation;

        std::vector<std::string> response;

        virtual explicit operator bool() const
        {
            return code == Code::OK;
        }

        virtual ~Result() = default;
    };
} // namespace at
