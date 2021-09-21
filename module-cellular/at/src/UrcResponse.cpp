// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <UrcResponse.hpp>
#include <Utils.hpp>

using namespace at::urc;

UrcResponse::URCResponseType UrcResponse::getURCResponseType() const noexcept
{
    return type;
}
