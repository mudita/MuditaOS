// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

namespace parserFSM::http
{
    /*! Enum class for the HTTP status codes.
     */
    enum class Code
    {
        OK                  = 200,
        Accepted            = 202,
        SeeOther            = 303,
        BadRequest          = 400,
        NotFound            = 404,
        NotAcceptable       = 406,
        InternalServerError = 500
    };

    /*! Enum class for the HTTP methods.
     */
    enum class Method
    {
        get = 1,
        post,
        put,
        del
    };

    [[nodiscard]] auto isMethodValid(uint8_t) -> bool;
}; // namespace parserFSM::http
