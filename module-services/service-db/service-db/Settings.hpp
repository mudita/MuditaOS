// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-sys/Service/Message.hpp>
#include <service-db/DBServiceName.hpp>
#include "SettingsScope.hpp"
#include "SettingsMessages.hpp"

#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>

namespace settings
{
    class SettingsCache;
    class Settings
    {
      public:
        using ValueChangedCallback           = std::function<void(const std::string &)>;
        using ValueChangedCallbackWithName   = std::function<void(const std::string &, const std::string &value)>;
        using ProfileChangedCallback         = std::function<void(const std::string &)>;
        using ModeChangedCallback            = ProfileChangedCallback;
        using ListOfProfiles                 = std::list<std::string>;
        using ListOfModes                    = ListOfProfiles;
        using OnAllProfilesRetrievedCallback = std::function<void(const ListOfProfiles &)>;
        using OnAllModesRetrievedCallback    = std::function<void(const ListOfModes &)>;

        Settings(sys::Service *app, const std::string &dbAgentName = service::name::db, SettingsCache *cache = nullptr);

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

        void getAllProfiles(OnAllProfilesRetrievedCallback cb);
        void setCurrentProfile(const std::string &profile);
        void addProfile(const std::string &profile);
        void registerProfileChange(ProfileChangedCallback);
        void unregisterProfileChange();

        void getAllModes(OnAllModesRetrievedCallback cb);
        void setCurrentMode(const std::string &mode);
        void addMode(const std::string &mode);
        void registerModeChange(ModeChangedCallback);
        void unregisterModeChange();

      private:
        std::string dbAgentName;

        std::shared_ptr<sys::Service> app;
        std::string serviceName;
        std::string phoneMode;
        std::string profile;

        SettingsCache *cache;

        using ValueCb = std::map<EntryPath, std::pair<ValueChangedCallback, ValueChangedCallbackWithName>>;
        ValueCb cbValues;
        ModeChangedCallback cbMode;
        OnAllModesRetrievedCallback cbAllModes;
        ProfileChangedCallback cbProfile;
        OnAllProfilesRetrievedCallback cbAllProfiles;
        void sendMsg(std::shared_ptr<settings::Messages::SettingsMessage> &&msg);
        void registerHandlers();
        auto handleVariableChanged(sys::Message *req) -> sys::MessagePointer;
        auto handleCurrentProfileChanged(sys::Message *req) -> sys::MessagePointer;
        auto handleCurrentModeChanged(sys::Message *req) -> sys::MessagePointer;
        auto handleProfileListResponse(sys::Message *req) -> sys::MessagePointer;
        auto handleModeListResponse(sys::Message *req) -> sys::MessagePointer;
    };
} // namespace settings
