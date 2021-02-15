// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BusProxy.hpp"

#include "details/bus/Bus.hpp"

namespace sys
{
    BusProxy::BusProxy(Service *owner) : owner{owner}, busImpl{std::make_unique<Bus>()}
    {
        channels.push_back(BusChannel::System); // Mandatory for each service.
    }

    BusProxy::~BusProxy() noexcept = default;

    bool BusProxy::sendUnicast(std::shared_ptr<Message> message, const std::string &targetName)
    {
        return busImpl->SendUnicast(std::move(message), targetName, owner);
    }

    SendResult BusProxy::sendUnicast(std::shared_ptr<Message> message, const std::string &targetName, uint32_t timeout)
    {
        return busImpl->SendUnicast(std::move(message), targetName, owner, timeout);
    }

    void BusProxy::sendMulticast(std::shared_ptr<Message> message, BusChannel channel)
    {
        busImpl->SendMulticast(std::move(message), channel, owner);
    }

    void BusProxy::sendBroadcast(std::shared_ptr<Message> message)
    {
        busImpl->SendBroadcast(std::move(message), owner);
    }

    void BusProxy::sendResponse(std::shared_ptr<Message> response, std::shared_ptr<Message> request)
    {
        busImpl->SendResponse(std::move(response), std::move(request), owner);
    }

    void BusProxy::connect()
    {
        Bus::Add(owner);
    }

    void BusProxy::disconnect()
    {
        Bus::Remove(owner);
    }
} // namespace sys
