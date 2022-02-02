// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Service/BusProxy.hpp>

#include "details/bus/Bus.hpp"

namespace sys
{
    BusProxy::BusProxy(Service *owner, Watchdog &watchdog)
        : owner{owner}, watchdog{watchdog}, busImpl{std::make_unique<Bus>()}
    {
        channels.push_back(BusChannel::System); // Mandatory for each service.
    }

    BusProxy::~BusProxy() noexcept = default;

    bool BusProxy::sendUnicast(std::shared_ptr<Message> message, const std::string &targetName)
    {
        auto ret = busImpl->SendUnicast(std::move(message), targetName, owner);
        if (ret) {
            watchdog.refresh();
        }
        return ret;
    }

    SendResult BusProxy::unicastSync(std::shared_ptr<Message> message, sys::Service*whose,  std::uint32_t timeout)
    {
        auto ret = busImpl->UnicastSync(message, whose, timeout);
        if (ret.first != ReturnCodes::Failure) {
            watchdog.refresh();
        }
        return ret;
    }

    SendResult BusProxy::sendUnicastSync(std::shared_ptr<Message> message,
                                         const std::string &targetName,
                                         uint32_t timeout)
    {
        auto ret = busImpl->SendUnicastSync(std::move(message), targetName, owner, timeout);
        if (ret.first != ReturnCodes::Failure) {
            watchdog.refresh();
        }
        return ret;
    }

    void BusProxy::sendMulticast(std::shared_ptr<Message> message, BusChannel channel)
    {
        busImpl->SendMulticast(std::move(message), channel, owner);
        watchdog.refresh();
    }

    void BusProxy::sendBroadcast(std::shared_ptr<Message> message)
    {
        busImpl->SendBroadcast(std::move(message), owner);
        watchdog.refresh();
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
