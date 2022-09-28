// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MmiActionsParams.hpp"

using namespace app::manager::actions;

void MMICustomResultParams::addMessage(const MMIResultMessage &message)
{
    messages.emplace_back(message);
}

auto MMICustomResultParams::getMessage() const -> std::vector<MMIResultMessage>
{
    return messages;
}

auto MMICustomResultParams::getMessageType() const noexcept -> MMIType
{
    return type;
}

void MMICustomResultParams::accept(Visitor &v, std::string &displayMessage)
{
    v.visit(*this, displayMessage);
};

void MMINoneSpecifiedResult::accept(Visitor &v, std::string &displayMessage)
{
    v.visit(*this, displayMessage);
};

auto MMICallForwardingResult::getData() const -> std::tuple<std::string, std::string, std::string, std::string>
{
    return std::make_tuple(voice, fax, sync, async);
}

void MMICallForwardingResult::accept(Visitor &v, std::string &displayMessage)
{
    v.visit(*this, displayMessage);
}

void MMICallBarringResult::accept(Visitor &v, std::string &displayMessage)
{
    v.visit(*this, displayMessage);
}

void MMICallWaitingResult::accept(Visitor &v, std::string &displayMessage)
{
    v.visit(*this, displayMessage);
}

void MMIClipResult::accept(Visitor &v, std::string &displayMessage)
{
    v.visit(*this, displayMessage);
}

void MMIClirResult::accept(Visitor &v, std::string &displayMessage)
{
    v.visit(*this, displayMessage);
}

void MMIImeiResult::accept(Visitor &v, std::string &displayMessage)
{
    v.visit(*this, displayMessage);
}

MMIParams::MMIParams(std::string mmiData) : mmiData{std::move(mmiData)}
{}

std::string MMIParams::getData() const
{
    return mmiData;
}

MMIResultParams::MMIResultParams(MMIResult result, std::shared_ptr<MMICustomResultParams> customResult)
    : result(result), customResult(std::move(customResult))
{}

MMIResultParams::MMIResult MMIResultParams::getData() const noexcept
{
    return result;
}

auto MMIResultParams::getCustomData() const noexcept -> std::shared_ptr<MMICustomResultParams>
{
    return customResult;
}

void MMICallBarringResult::addMessages(const std::pair<MMIResultMessage, MMIResultMessage> &message) noexcept
{
    data.push_back(message);
}

auto MMICallBarringResult::getMessages(void) noexcept -> std::vector<std::pair<MMIResultMessage, MMIResultMessage>>
{
    return data;
}

auto MMIImeiResult::getImei() const noexcept -> std::string
{
    return imei;
}

void MMICallWaitingResult::addMessages(const std::pair<MMIResultMessage, MMIResultMessage> &message)
{
    messages.push_back(message);
}

auto MMICallWaitingResult::getMessages() const noexcept -> std::vector<std::pair<MMIResultMessage, MMIResultMessage>>
{
    return messages;
}
