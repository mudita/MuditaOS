// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ImeiGetHandler.hpp"
#include <ATFactory.hpp>
#include <modem/BaseChannel.hpp>

#include <log/Logger.hpp>

namespace
{
    constexpr auto IMEIResonseSize = 2;
}

namespace cellular::service
{
    void ImeiGetHandler::setChannel(at::BaseChannel *channel)
    {
        this->channel = channel;
    }

    auto ImeiGetHandler::getImei(std::string &destination) -> bool
    {

        if (channel == nullptr) {
            LOG_ERROR("No channel provided. Request ignored");
            return false;
        }
        auto result = channel->cmd(at::factory(at::AT::GET_IMEI));

        if (result.code == at::Result::Code::OK && result.response.size() == IMEIResonseSize) {
            destination = result.response[0];
            return true;
        }
        else {
            return false;
        }
    }

} // namespace cellular::service
