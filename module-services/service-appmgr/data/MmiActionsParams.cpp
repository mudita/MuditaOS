// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-appmgr/data/MmiActionsParams.hpp>

using namespace app::manager::actions;

MMIParams::MMIParams(std::string mmiData) : mmiData{std::move(mmiData)}
{}

std::string MMIParams::getData() const
{
    return mmiData;
}

MMIResultParams::MMIResultParams(MMIResult result) : result(result)
{}

MMIResultParams::MMIResult MMIResultParams::getData() const noexcept
{
    return result;
}
