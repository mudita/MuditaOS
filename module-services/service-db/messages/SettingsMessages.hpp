#pragma once

#include <Service/Message.hpp>
#include <MessageType.hpp>

#include <memory>
#include <utility>
#include <variant>
#include <set>

namespace Settings
{
    struct EntryPath
    {
        std::string mode;
        std::string service;
        std::string profile;
        std::string variable;

        [[nodiscard]] auto to_string(std::string sep = "\\") const -> std::string
        {
            return mode + sep + service + sep + profile + sep + variable;
        }
    };

    namespace Messages
    {
        class SettingsMessage : public sys::DataMessage
        {
          public:
            explicit SettingsMessage() : sys::DataMessage(MessageType::Settings){};
            ~SettingsMessage() override = default;
        };

        /// Variable manipulation
        class VariableSettingsMessage : public SettingsMessage
        {
          public:
            VariableSettingsMessage() : SettingsMessage()
            {}

            explicit VariableSettingsMessage(EntryPath path,
                                             std::optional<std::string> value = std::make_optional<std::string>())
                : SettingsMessage(), path(std::move(path)), value(std::move(value))
            {}

            [[nodiscard]] auto getValue() const -> std::optional<std::string>
            {
                return value;
            }

            [[nodiscard]] auto getPath() const -> EntryPath
            {
                return path;
            }

          protected:
            EntryPath path;
            std::optional<std::string> value;
        };

        class GetVariable : public VariableSettingsMessage
        {
          public:
            GetVariable() = default;
            explicit GetVariable(EntryPath path) : VariableSettingsMessage(path)
            {}
            ~GetVariable() override = default;
        };

        class SetVariable : public VariableSettingsMessage
        {
          public:
            SetVariable() = default;
            SetVariable(EntryPath path, std::string value) : VariableSettingsMessage(path, value)
            {}
        };

        class RegisterOnVariableChange : public VariableSettingsMessage
        {
          public:
            RegisterOnVariableChange() = default;
            explicit RegisterOnVariableChange(EntryPath path) : VariableSettingsMessage(path)
            {}
        };

        class UnregisterOnVariableChange : public VariableSettingsMessage
        {
          public:
            UnregisterOnVariableChange() = default;
            explicit UnregisterOnVariableChange(EntryPath path) : VariableSettingsMessage(path)
            {}
        };

        class SettingsVariableChanged : public VariableSettingsMessage
        {
          public:
            SettingsVariableChanged() = default;
            explicit SettingsVariableChanged(EntryPath path, std::string value, std::string old_value)
                : VariableSettingsMessage(path, value), old_value(std::move(old_value))
            {}

            [[nodiscard]] auto getOldValue() const -> std::string
            {
                return old_value;
            }

            std::string old_value;
        };

        /// Profiles manipulation
        class ListProfiles : public SettingsMessage
        {
          public:
            auto getProfiles() -> const std::set<std::string> &
            {
                return profiles;
            }

          private:
            std::set<std::string> profiles;
        };

        class ProfileSettingsMessage : public SettingsMessage
        {
          public:
            auto getProfileName() -> std::string
            {
                return profile;
            }

          protected:
            ProfileSettingsMessage() = default;
            explicit ProfileSettingsMessage(std::string name) : SettingsMessage(), profile(std::move(name))
            {}

          protected:
            std::string profile;
        };

        class SetCurrentProfile : public ProfileSettingsMessage
        {
          public:
            SetCurrentProfile() = default;
            explicit SetCurrentProfile(std::string profile) : ProfileSettingsMessage(profile)
            {}
        };

        class AddProfile : public ProfileSettingsMessage
        {
          public:
            AddProfile() = default;
            explicit AddProfile(std::string profile) : ProfileSettingsMessage(profile)
            {}
        };

        class GetCurrentProfile : public ProfileSettingsMessage
        {
          public:
            GetCurrentProfile() = default;
            explicit GetCurrentProfile(std::string profile) : ProfileSettingsMessage(profile)
            {}
        };

        class RegisterOnProfileChange : public SettingsMessage
        {
          public:
            RegisterOnProfileChange()           = default;
            ~RegisterOnProfileChange() override = default;
        };

        class UnregisterOnProfileChange : public SettingsMessage
        {
          public:
            UnregisterOnProfileChange()           = default;
            ~UnregisterOnProfileChange() override = default;
        };

        class CurrentProfileChanged : public ProfileSettingsMessage
        {
          public:
            CurrentProfileChanged() = default;
            explicit CurrentProfileChanged(std::string profile) : ProfileSettingsMessage(profile)
            {}
        };

        /// Modes manipulation
        class ListModes : public SettingsMessage
        {
          public:
            auto getModes() -> const std::set<std::string> &
            {
                return modes;
            }

          private:
            std::set<std::string> modes;
        };

        class ModeSettingsMessage : public SettingsMessage
        {
          public:
            auto getModeName() -> std::string
            {
                return mode;
            }

          protected:
            ModeSettingsMessage() = default;
            explicit ModeSettingsMessage(std::string mode) : SettingsMessage(), mode(std::move(mode))
            {}

          protected:
            std::string mode;
        };

        class SetCurrentMode : public ProfileSettingsMessage
        {
          public:
            SetCurrentMode() = default;
            explicit SetCurrentMode(std::string mode) : ProfileSettingsMessage(mode)
            {}
        };

        class AddMode : public ProfileSettingsMessage
        {
          public:
            AddMode() = default;
            explicit AddMode(std::string mode) : ProfileSettingsMessage(mode)
            {}
        };

        class GetCurrentMode : public ProfileSettingsMessage
        {
          public:
            GetCurrentMode() = default;
            explicit GetCurrentMode(std::string profile) : ProfileSettingsMessage(profile)
            {}
        };

        class RegisterOnModeChange : public SettingsMessage
        {
          public:
            RegisterOnModeChange()           = default;
            ~RegisterOnModeChange() override = default;
        };

        class UnregisterOnModeChange : public SettingsMessage
        {
          public:
            UnregisterOnModeChange()           = default;
            ~UnregisterOnModeChange() override = default;
        };

        class CurrentModeChanged : public ProfileSettingsMessage
        {
          public:
            CurrentModeChanged() = default;
            explicit CurrentModeChanged(std::string profile) : ProfileSettingsMessage(profile)
            {}
        };

        // Response handling
        class SettingsResponseMessage : public sys::ResponseMessage
        {};

        class VariableSettingsResponse : public sys::ResponseMessage
        {
          public:
            explicit VariableSettingsResponse(EntryPath path,
                                              std::optional<std::string> value,
                                              sys::ReturnCodes code = sys::ReturnCodes::Success)
                : sys::ResponseMessage(code), path(std::move(path)), value(std::move(value))
            {}

            [[nodiscard]] auto getValue() const -> std::optional<std::string>
            {
                return value;
            }

            [[nodiscard]] auto getPath() const -> std::string
            {
                return path.to_string();
            }

          protected:
            EntryPath path;
            std::optional<std::string> value;
        };

    } // namespace Messages
} // namespace Settings
