#pragma once

#include <list>
#include <functional>
#include <map>
#include <memory>
#include <string>

#include <includes/DBServiceName.hpp>

namespace sys
{
    class Service;
    class Message;
    using Message_t = std::shared_ptr<Message>;
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

        Settings(sys::Service *app, const std::string &dbAgentName = service::name::db);
        virtual ~Settings();
        std::optional<std::string> getValue(const std::string &variableName);
        void setValue(const std::string &variableName, const std::string &variableValue);
        void registerValueChange(const std::string &variableName, ValueChangedCallback cb);
        void unregisterValueChange(const std::string &variableName);

        ListOfProfiles getAllProfiles();
        std::string getCurrentProfile();
        void setCurrentProfile(const std::string &profile);
        void addProfile(const std::string &profile);
        void registerProfileChange(ProfileChangedCallback);
        void unregisterProfileChange();

        ListOfModes getAllModes();
        std::string getCurrentMode();
        void setCurrentMode(const std::string &mode);
        void addMode(const std::string &mode);
        void registerModeChange(ModeChangedCallback);
        void unregisterModeChange();

      private:
        std::string dbAgentName;
        constexpr static int16_t dbWaitTimeMs = 5000;

        std::shared_ptr<sys::Service> app;
        std::string serviceName;
        std::string phoneMode;
        std::string profile;

        using ValueCb = std::map<std::string, ValueChangedCallback>;
        ValueCb cbValues;
        ModeChangedCallback cbMode;
        ProfileChangedCallback cbProfile;
        void sendMsg(std::shared_ptr<::Settings::Messages::SettingsMessage> &&msg);
        sys::Message_t sendMsgAndWaitForResponse(std::shared_ptr<::Settings::Messages::SettingsMessage> &&msg);
        void registerHandlers();
        void handleValueChgMsg();
        void handleProfileChgMsg();
        void handleModeChgMsg();
    };
} // namespace Settings
