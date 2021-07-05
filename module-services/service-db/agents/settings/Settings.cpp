// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/Settings.hpp>
#include <service-db/SettingsCache.hpp>

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#if defined(DEBUG_SETTINGS_DB) and DEBUG_SETTINGS_DB == 1
#include <log.hpp>
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

    void Settings::init(const service::ServiceProxy &interface)
    {
        this->interface = SettingsProxy(interface);
        this->interface.init([this](const EntryPath &p, const std::string &v) { this->handleVariableChanged(p, v); });
        if (not interface.isValid()) {
            throw std::invalid_argument("need the interface!");
        }
    }

    void Settings::deinit()
    {
        if (interface.isValid()) {
            interface.deinit();
            for (const auto &callbacks : cbValues) {
                log_debug("[Settings::unregisterValueChange] %s", callbacks.first.to_string().c_str());
                interface.unregisterValueChange(callbacks.first);
            }
        }
        cbValues.clear();
    }

    void Settings::handleVariableChanged(const EntryPath &path, const std::string &value)
    {
        log_debug("handleVariableChanged: (key=%s)", path.to_string().c_str());
        if (auto callbacks = cbValues.find(path); std::end(cbValues) != callbacks) {
            // we cant get by const ref here - as user can remove this callback from cbValues map which would cause use
            // after free
            auto [cb, cbWithName] = callbacks->second;
            if (nullptr != cb) {
                cb(value);
            }
            if (nullptr != cbWithName) {
                cbWithName(path.variable, value);
            }
        }
    }

    void Settings::registerValueChange(const std::string &variableName, ValueChangedCallback cb, SettingsScope scope)
    {
        auto path = EntryPath{.service = interface.ownerName(), .variable = variableName, .scope = scope};

        if (auto callbacks = cbValues.find(path);
            std::end(cbValues) != callbacks && nullptr != callbacks->second.first) {
            log_debug("Callback function on value change (%s) already exists, rewriting", path.to_string().c_str());
        }
        cbValues[path].first = std::move(cb);
        interface.registerValueChange(std::move(path));
    }

    void Settings::registerValueChange(const std::string &variableName,
                                       ValueChangedCallbackWithName cb,
                                       SettingsScope scope)
    {
        auto path = EntryPath{.service = interface.ownerName(), .variable = variableName, .scope = scope};

        if (auto callbacks = cbValues.find(path); cbValues.end() != callbacks && nullptr != callbacks->second.second) {
            log_debug("Callback function on value change (%s) already exists, rewriting", path.to_string().c_str());
        }
        cbValues[path].second = std::move(cb);
        interface.registerValueChange(std::move(path));
    }

    void Settings::unregisterValueChange(const std::string &variableName, SettingsScope scope)
    {
        auto path = EntryPath{.service = interface.ownerName(), .variable = variableName, .scope = scope};

        if (auto callbacks = cbValues.find(path); cbValues.end() != callbacks) {
            log_debug("[Settings::unregisterValueChange] %s", path.to_string().c_str());
            cbValues.erase(callbacks);
        }
        interface.unregisterValueChange(std::move(path));
    }

    void Settings::setValue(const std::string &variableName, const std::string &variableValue, SettingsScope scope)
    {
        auto path = EntryPath{.service = interface.ownerName(), .variable = variableName, .scope = scope};
        interface.setValue(path, variableValue);
        getCache()->setValue(path, variableValue);
    }

    std::string Settings::getValue(const std::string &variableName, SettingsScope scope)
    {
        return getCache()->getValue({.service = interface.ownerName(), .variable = variableName, .scope = scope});
    }

    SettingsCache *Settings::getCache()
    {
        return SettingsCache::getInstance();
    }

} // namespace settings
