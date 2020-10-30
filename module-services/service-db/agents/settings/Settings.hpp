// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <includes/DBServiceName.hpp> // for db
#include <stdint.h>                   // for int16_t
#include <list>                       // for list
#include <functional>                 // for function
#include <map>                        // for map
#include <memory>                     // for shared_ptr, allocator
#include <string>                     // for string
#include <optional>                   // for optional

namespace sys
{
    class Service;
    class Message;

    using Message_t = std::shared_ptr<Message>;
    class DataMessage;
    class ResponseMessage;
}; // namespace sys

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
        auto handleVariableChanged(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
        auto handleCurrentProfileChanged(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
        auto handleCurrentModeChanged(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
        auto handleProfileListResponse(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
        auto handleModeListResponse(sys::DataMessage *req, sys::ResponseMessage *) -> sys::Message_t;
    };
} // namespace Settings
