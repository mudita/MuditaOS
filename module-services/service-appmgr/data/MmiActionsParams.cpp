// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-appmgr/data/MmiActionsParams.hpp>

using namespace app::manager::actions;

void MMICustomResultParams::addMessage(const MMIResultMessage &message)
{
    messages.push_back(message);
}

auto MMICustomResultParams::getMessage() -> std::vector<MMIResultMessage>
{
    return std::move(messages);
}

auto MMICustomResultParams::getType() -> MMIType
{
    return type;
}

MMIParams::MMIParams(std::string mmiData) : mmiData{std::move(mmiData)}
{}

std::string MMIParams::getData() const
{
    return mmiData;
}

MMIResultParams::MMIResultParams(MMIResult result) : result(result)
{}

MMIResultParams::MMIResultParams(std::shared_ptr<IMMICustomResultParams> cResult) : customResult(cResult)
{}

MMIResultParams::MMIResult MMIResultParams::getData() const noexcept
{
    return result;
}

auto MMIResultParams::getCustomMessage() const noexcept -> std::shared_ptr<IMMICustomResultParams>
{
    return customResult;
}
