// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

namespace sdesktop::endpoints::http
{
    /*! Enum class for the HTTP status codes.
     */
    enum class Code
    {
        OK                  = 200,
        Created             = 201,
        Accepted            = 202,
        NoContent           = 204,
        SeeOther            = 303,
        BadRequest          = 400,
        Forbidden           = 403,
        NotFound            = 404,
        NotAcceptable       = 406,
        Conflict            = 409,
        UnprocessableEntity = 422,
        InternalServerError = 500,
        NotImplemented      = 501,
        InsufficientStorage = 507
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
}; // namespace sdesktop::endpoints::http
