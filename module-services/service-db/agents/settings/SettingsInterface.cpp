// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/Settings.hpp>
#include <service-db/SettingsCache.hpp>
#include <Service/ServiceInterface.hpp>
#include <Service/Service.hpp>

namespace settings
{
    Interface::Interface(const service::Interface &interface) : service::Interface(interface)
    {}

    std::string Interface::ownerName()
    {
        return getApp()->GetName();
    }

    void Interface::subscribeDB()
    {
        getApp()->bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
    }

    SettingsCache *Interface::getCache()
    {
        return SettingsCache::getInstance();
    }

    void Interface::changeHandler(const std::type_info &what, Change change, sys::MessageHandler foo)
    {
        switch (change) {
        case Change::Register:
            getApp()->connect(what, std::move(foo));
            break;
        case Change::Deregister:
            getApp()->disconnect(what);
            break;
        }
    }

    void Interface::sendMsg(std::shared_ptr<settings::Messages::SettingsMessage> &&msg)
    {
        getApp()->bus.sendUnicast(std::move(msg), agentName());
    }

    Interface::operator bool() noexcept
    {
        return isValid();
    }
} // namespace settings
