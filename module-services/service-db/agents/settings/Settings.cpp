// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <memory>
#include <service-db/Settings.hpp>
#include <service-db/SettingsMessages.hpp>
#include <service-db/SettingsCache.hpp>

#include <Service/Common.hpp>
#include <Service/Message.hpp>
//#include <Service/Service.hpp>
// #include <log/log.hpp>

#include <stdexcept>
#include <utility>
#include <vector>

#if defined(DEBUG_SETTINGS_DB) and DEBUG_SETTINGS_DB == 1
#define log_debug(...) LOG_DEBUG(__VA_ARGS__);
#else
#define log_debug(...)
#endif

namespace settings
{
    Settings::~Settings()
    {
        deinit();
    }

    void Settings::init(const service::Interface &interface)
    {
        this->interface = Interface(interface);
        if (this->interface) {
            changeHandlers(Interface::Change::Register);
        }
        else {
            throw std::runtime_error("need the interface!");
        }
    }

    void Settings::deinit()
    {
        if (interface) {
            changeHandlers(Interface::Change::Deregister);
        }
    }

    void Settings::changeHandlers(enum Interface::Change change)
    {
        using std::placeholders::_1;
        using std::placeholders::_2;
        interface.changeHandler(
            typeid(settings::Messages::VariableChanged), change, std::bind(&Settings::handleVariableChanged, this, _1));
    }

    auto Settings::handleVariableChanged(sys::Message *req) -> sys::MessagePointer
    {
        log_debug("handleVariableChanged");
        if (auto msg = dynamic_cast<settings::Messages::VariableChanged *>(req)) {
            auto key = msg->getPath();
            auto val = msg->getValue();
            log_debug("handleVariableChanged: (k=v): (%s=%s)", key.to_string().c_str(), val.value_or("").c_str());
            ValueCb::iterator it_cb = cbValues.find(key);
            if (cbValues.end() != it_cb) {
                auto [cb, cbWithName] = it_cb->second;
                if (nullptr != cb && nullptr != cbWithName) {
                    // in case of two callbacks there is a need to duplicate the value
                    auto value = val.value_or("");
                    cb(std::string{value});
                    cbWithName(key.variable, value);
                }
                else if (nullptr != cb) {
                    cb(val.value_or(""));
                }
                else {
                    cbWithName(key.variable, val.value_or(""));
                }
            }
        }
        return std::make_shared<sys::ResponseMessage>();
    }

    void Settings::registerValueChange(const std::string &variableName, ValueChangedCallback cb, SettingsScope scope)
    {
        auto path = EntryPath{.service = interface.ownerName(), .variable = variableName, .scope = scope};

        auto it_cb = cbValues.find(path);
        if (cbValues.end() != it_cb && nullptr != it_cb->second.first) {
            // LOG_INFO("Callback function on value change (%s) already exists, rewriting", path.to_string().c_str());
        }
        cbValues[path].first = std::move(cb);

        auto msg = std::make_shared<settings::Messages::RegisterOnVariableChange>(path);
        interface.sendMsg(std::move(msg));
    }

    void Settings::registerValueChange(const std::string &variableName,
                                       ValueChangedCallbackWithName cb,
                                       SettingsScope scope)
    {
        auto path = EntryPath{.service = interface.ownerName(), .variable = variableName, .scope = scope};

        auto it_cb = cbValues.find(path);
        if (cbValues.end() != it_cb && nullptr != it_cb->second.second) {
            // LOG_INFO("Callback function on value change (%s) already exists, rewriting", path.to_string().c_str());
        }
        cbValues[path].second = std::move(cb);

        auto msg = std::make_shared<settings::Messages::RegisterOnVariableChange>(path);
        interface.sendMsg(std::move(msg));
    }

    void Settings::unregisterValueChange(const std::string &variableName, SettingsScope scope)
    {
        auto path = EntryPath{.service = interface.ownerName(), .variable = variableName, .scope = scope};

        auto it_cb = cbValues.find(path);
        if (cbValues.end() == it_cb) {
            // LOG_INFO("Callback function on value change (%s) does not exist", path.to_string().c_str());
        }
        else {
            log_debug("[Settings::unregisterValueChange] %s", path.to_string().c_str());
            cbValues.erase(it_cb);
        }

        auto msg = std::make_shared<settings::Messages::UnregisterOnVariableChange>(path);
        interface.sendMsg(std::move(msg));
    }

    void Settings::unregisterValueChange()
    {
        for (const auto &it_cb : cbValues) {
            log_debug("[Settings::unregisterValueChange] %s", it_cb.first.to_string().c_str());
            auto msg = std::make_shared<settings::Messages::UnregisterOnVariableChange>(it_cb.first);
            interface.sendMsg(std::move(msg));
        }
        cbValues.clear();
        // LOG_INFO("Unregistered all settings variable change on service (%s)", interface.ownerName().c_str());
    }

    void Settings::setValue(const std::string &variableName, const std::string &variableValue, SettingsScope scope)
    {
        auto path = EntryPath{.service = interface.ownerName(), .variable = variableName, .scope = scope};
        interface.sendMsg(std::make_shared<settings::Messages::SetVariable>(path, variableValue));
        interface.getCache()->setValue(path, variableValue);
    }

    std::string Settings::getValue(const std::string &variableName, SettingsScope scope)
    {
        return interface.getCache()->getValue(
            {.service = interface.ownerName(), .variable = variableName, .scope = scope});
    }
} // namespace settings
