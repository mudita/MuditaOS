// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <URC_POWERED_DOWN.hpp>
#include <module-utils/Utils.hpp>

using namespace at::urc;

bool POWERED_DOWN::isValid() const noexcept
{
    return isNormalPowerDown() || isImmediatePowerDown();
}

bool POWERED_DOWN::isNormalPowerDown() const noexcept
{
    return urcBody.find(POWERED_DOWN::head_normal) != std::string::npos;
}

bool POWERED_DOWN::isImmediatePowerDown() const noexcept
{
    return urcBody.find(POWERED_DOWN::head_immediate) != std::string::npos;
}
