// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <URC_RESPONSE.hpp>
#include <module-utils/Utils.hpp>

using namespace at::urc;

std::unique_ptr<URC_RESPONSE> URC_RESPONSE::Create(const std::string body)
{
    std::string trimmeBody = utils::trim(body);
    int idx                = 0;
    for (std::string_view respBody : responseBodies) {
        if (respBody == trimmeBody) {
            return std::make_unique<URC_RESPONSE>(magic_enum::enum_value<URCResponseType>(idx));
        }
        idx++;
    }
    return nullptr;
}

URC_RESPONSE::URCResponseType URC_RESPONSE::getURCResponseType() const noexcept
{
    return type;
}
