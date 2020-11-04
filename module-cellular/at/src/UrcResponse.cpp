// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <UrcResponse.hpp>
#include <module-utils/Utils.hpp>

using namespace at::urc;

/*std::unique_ptr<UrcResponse> UrcResponse::Create(const std::string body)
{
    std::string trimmeBody = utils::trim(body);
    int idx                = 0;
    for (auto& respBody : responseBodies) {
        if (respBody.second == trimmeBody) {
            return std::make_unique<UrcResponse>(respBody.first);
        }
        idx++;
    }
    return nullptr;
}*/

UrcResponse::URCResponseType UrcResponse::getURCResponseType() const noexcept
{
    return type;
}
