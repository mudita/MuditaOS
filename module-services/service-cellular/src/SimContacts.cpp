// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimContacts.hpp"

#include <modem/BaseChannel.hpp>
#include <at/cmd/CPBS.hpp>
#include <at/cmd/CPBR.hpp>

#include <log/Logger.hpp>
namespace cellular::service
{
    void SimContacts::setChannel(at::BaseChannel *channel)
    {
        this->channel = channel;
    }

    auto SimContacts::getContacts(std::vector<cellular::SimContact> &destination) -> bool
    {
        constexpr auto firstIndex = 1;
        if (channel == nullptr) {
            LOG_ERROR("No channel provided. Request ignored");
            return false;
        }
        auto contactsCount = 0;
        if (!getContactCount(contactsCount)) {
            return false;
        }

        auto command = at::cmd::CPBR(at::cmd::Modifier::Set);
        command.setSimContactsReadRange(firstIndex, contactsCount);
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

    auto SimContacts::getContactCount(int &count) -> bool
    {
        if (channel == nullptr) {
            LOG_ERROR("No channel provided. Request ignored");
            return false;
        }

        auto command = at::cmd::CPBS(at::cmd::Modifier::Set);
        command.set();
        auto response = channel->cmd(command);

        if (response.code != at::Result::Code::OK) {
            LOG_ERROR("Failed to set contact storage");
            return false;
        }

        command     = at::cmd::CPBS(at::cmd::Modifier::Get);
        response    = channel->cmd(command);
        auto result = command.parseCPBS(response);

        if (result.code != at::Result::Code::OK) {
            LOG_ERROR("Failed to get contacts count");
            return false;
        }

        count = result.used;
        return true;
    }
} // namespace cellular::service
