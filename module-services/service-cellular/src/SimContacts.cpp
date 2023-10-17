// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimContacts.hpp"

#include <modem/BaseChannel.hpp>
#include <at/cmd/CPBR.hpp>

namespace cellular::service
{
    void SimContacts::setChannel(at::BaseChannel *channel)
    {
        this->channel = channel;
    }

    auto SimContacts::getContacts(std::vector<cellular::SimContact> &destination) -> bool
    {
        if (!channel) {
            LOG_ERROR("No channel provided. Request ignored");
            return false;
        }

        auto command = at::cmd::CPBR(at::cmd::Modifier::Set);
        constexpr int firstIndex = 1, maxIndex = 250;
        command.setSimContactsReadRange(firstIndex, maxIndex);
        auto response = channel->cmd(command);
        auto result   = command.parseCPBR(response);

        if (result.code != at::Result::Code::OK) {
            LOG_ERROR("Failed to get contacts list");
            return false;
        }

        for (auto el : result.contacts) {
            destination.push_back(SimContact(el.name, el.number));
        }

        return true;
    }
} // namespace cellular::service
