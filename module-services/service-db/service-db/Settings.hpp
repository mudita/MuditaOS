// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DBServiceName.hpp"
#include "SettingsScope.hpp"
#include "SettingsInterface.hpp"

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
    class Settings
    {
      public:
        using ValueChangedCallback           = std::function<void(const std::string &)>;
        using ValueChangedCallbackWithName   = std::function<void(const std::string &, const std::string &value)>;

        Settings() = default;
        virtual ~Settings();

        void init(const service::Interface &interface);
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
        void unregisterValueChange();
        std::string getValue(const std::string &variableName, SettingsScope scope = SettingsScope::AppLocal);

      private:
        Interface interface;
        std::string serviceName;
        std::string phoneMode;
        std::string profile;


        using ValueCb = std::map<EntryPath, std::pair<ValueChangedCallback, ValueChangedCallbackWithName>>;
        ValueCb cbValues;

        /// using owner service either
        /// - to register handlers
        /// - to unregister handlers
        /// handlers are called per service if for some reason service will stop
        /// existing - handlers shouldn't be called
        void changeHandlers(enum Interface::Change change);
        auto handleVariableChanged(sys::Message *req) -> sys::MessagePointer;
    };
} // namespace settings
