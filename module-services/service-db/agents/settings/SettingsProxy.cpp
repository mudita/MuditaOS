// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/EntryPath.hpp>
#include <service-db/SettingsMessages.hpp>
#include <service-db/Settings.hpp>
#include <service-db/SettingsCache.hpp>
#include <service-db/DBServiceName.hpp>
#include <Service/ServiceProxy.hpp>
#include <Service/Service.hpp>
#include <utility>

namespace settings
{

    SettingsProxy::SettingsProxy(const service::ServiceProxy &interface) : service::ServiceProxy(interface)
    {}

    SettingsProxy::~SettingsProxy()
    {
        deinit();
    }

    std::string SettingsProxy::ownerName()
    {
        return getService()->GetName();
    }

    void SettingsProxy::init(std::function<void(EntryPath, std::string)> onChangeHandler)
    {
        this->onChangeHandler = std::move(onChangeHandler);

        getService()->bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
        getService()->connect(typeid(settings::Messages::VariableChanged), [this](sys::Message *req) {
            if (auto msg = dynamic_cast<settings::Messages::VariableChanged *>(req)) {
                onChange(msg->getPath(), msg->getValue().value_or(""));
            }
            return std::make_shared<sys::ResponseMessage>();
        });
    }

    void SettingsProxy::deinit()
    {
        if (isValid()) {
            getService()->disconnect(typeid(settings::Messages::VariableChanged));
        }
    }

    [[nodiscard]] bool SettingsProxy::isValid() const noexcept
    {
        return service::ServiceProxy::isValid();
    }

    void SettingsProxy::onChange(EntryPath path, std::string value)
    {
        if (onChangeHandler) {
            onChangeHandler(std::move(path), std::move(value));
        }
    }

    template <typename T, typename... Args>
    auto message(sys::BusProxy &bus, Args... args)
    {
        bus.sendUnicast(std::make_shared<T>(args...), service::name::db);
    }

    void SettingsProxy::registerValueChange(EntryPath path)
    {
        message<settings::Messages::RegisterOnVariableChange>(getService()->bus, std::move(path));
    }

    void SettingsProxy::unregisterValueChange(EntryPath path)
    {
        message<settings::Messages::UnregisterOnVariableChange>(getService()->bus, std::move(path));
    }

    void SettingsProxy::setValue(const EntryPath &path, const std::string &value)
    {
        message<settings::Messages::SetVariable>(getService()->bus, path, value);
    }

} // namespace settings
