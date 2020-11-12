// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-sys/Service/Message.hpp>

#include <service-db/DBServiceName.hpp>

#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>

namespace Settings
{
    namespace Messages
    {
        class SettingsMessage;
    }

    class Settings
    {
      public:
        using ValueChangedCallback   = std::function<void(const std::string &, std::optional<std::string>)>;
        using ProfileChangedCallback = std::function<void(const std::string &)>;
        using ModeChangedCallback    = ProfileChangedCallback;
        using ListOfProfiles         = std::list<std::string>;
        using ListOfModes            = ListOfProfiles;
        using OnAllProfilesRetrievedCallback = std::function<void(const ListOfProfiles &)>;
        using OnAllModesRetrievedCallback    = std::function<void(const ListOfModes &)>;

        Settings(sys::Service *app, const std::string &dbAgentName = service::name::db);
        virtual ~Settings();

        void setValue(const std::string &variableName, const std::string &variableValue);
        void registerValueChange(const std::string &variableName, ValueChangedCallback cb);
        void unregisterValueChange(const std::string &variableName);

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

        using ValueCb = std::map<std::string, ValueChangedCallback>;
        ValueCb cbValues;
        ModeChangedCallback cbMode;
        OnAllModesRetrievedCallback cbAllModes;
        ProfileChangedCallback cbProfile;
        OnAllProfilesRetrievedCallback cbAllProfiles;
        void sendMsg(std::shared_ptr<::Settings::Messages::SettingsMessage> &&msg);
        void registerHandlers();
        auto handleVariableChanged(sys::Message *req) -> sys::MessagePointer;
        auto handleCurrentProfileChanged(sys::Message *req) -> sys::MessagePointer;
        auto handleCurrentModeChanged(sys::Message *req) -> sys::MessagePointer;
        auto handleProfileListResponse(sys::Message *req) -> sys::MessagePointer;
        auto handleModeListResponse(sys::Message *req) -> sys::MessagePointer;
    };
} // namespace Settings
