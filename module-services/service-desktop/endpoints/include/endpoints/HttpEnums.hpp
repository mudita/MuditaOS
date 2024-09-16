// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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
        Locked              = 423,
        InternalServerError = 500,
        NotImplemented      = 501,
        InsufficientStorage = 507
    };

    /*! Enum class for the HTTP methods.
     */
    enum class Method : std::uint8_t
    {
        Get = 1,
        Post,
        Put,
        Del
    };

    [[nodiscard]] auto isMethodValid(std::uint8_t method) -> bool;
}; // namespace sdesktop::endpoints::http
