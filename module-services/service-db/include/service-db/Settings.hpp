// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EntryPath.hpp"
#include "SettingsScope.hpp"
#include "SettingsProxy.hpp"

#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <exception>

namespace settings
{
    class SettingsCache;

    class Settings
    {
      public:
        using ValueChangedCallback         = std::function<void(const std::string &)>;
        using ValueChangedCallbackWithName = std::function<void(const std::string &, const std::string &value)>;

        explicit Settings(const service::ServiceProxy &interface);
        Settings() = default;
        virtual ~Settings();

        void init(const service::ServiceProxy &interface);
        void deinit();

        void setValue(const std::string &variableName,
                      const std::string &variableValue,
                      SettingsScope scope = SettingsScope::AppLocal);
        void registerValueChange(const std::string &variableName,
                                 ValueChangedCallback cb,
                                 SettingsScope scope = SettingsScope::AppLocal);
        void registerValueChange(const std::string &variableName,
                                 ValueChangedCallbackWithName cb,
                                 SettingsScope scope = SettingsScope::AppLocal);
        void unregisterValueChange(const std::string &variableName, SettingsScope scope = SettingsScope::AppLocal);
        /// unregisters all registered variables (both global and local)
        std::string getValue(const std::string &variableName, SettingsScope scope = SettingsScope::AppLocal);

        SettingsCache *getCache();

      private:
        SettingsProxy interface;

        using ValueCb = std::map<EntryPath, std::pair<ValueChangedCallback, ValueChangedCallbackWithName>>;

        ValueCb cbValues;

        void handleVariableChanged(const EntryPath &path, const std::string &value);
    };
} // namespace settings
