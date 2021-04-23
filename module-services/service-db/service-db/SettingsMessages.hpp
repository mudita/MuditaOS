// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>
#include <service-db/SettingsScope.hpp>

#include <list>
#include <memory>
#include <set>
#include <utility>
#include <variant>

namespace settings
{
    struct EntryPath
    {
        std::string mode = "undefined";
        std::string service;
        std::string profile = "undefined";
        std::string variable;
        SettingsScope scope;

        [[nodiscard]] auto to_string(std::string sep = "\\") const -> std::string
        {
            if (SettingsScope::Global == scope) {
                return variable;
            }
            return mode + sep + service + sep + profile + sep + variable;
        }

        void parse(const std::string &dbPath);
    };

    bool operator<(const EntryPath &lhs, const EntryPath &rhs) noexcept;

    namespace Messages
    {
        class SettingsMessage : public sys::DataMessage
        {
          public:
            explicit SettingsMessage(MessageType type = MessageType::Settings) : sys::DataMessage(type){};
            ~SettingsMessage() override = default;
        };

        /// Variable manipulation
        class Variable : public SettingsMessage
        {
          public:
            Variable() = default;
            explicit Variable(EntryPath path, std::optional<std::string> value = {})
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

        class GetVariable : public Variable
        {
          public:
            GetVariable() = default;
            explicit GetVariable(EntryPath path) : Variable(path)
            {}
        };

        class SetVariable : public Variable
        {
          public:
            SetVariable() = default;
            SetVariable(EntryPath path, std::string value) : Variable(path, value)
            {}
        };

        class RegisterOnVariableChange : public Variable
        {
          public:
            RegisterOnVariableChange() = default;
            explicit RegisterOnVariableChange(EntryPath path) : Variable(path)
            {}
        };

        class UnregisterOnVariableChange : public Variable
        {
          public:
            UnregisterOnVariableChange() = default;
            explicit UnregisterOnVariableChange(EntryPath path) : Variable(path)
            {}
        };

        class VariableChanged : public Variable
        {
          public:
            VariableChanged() = default;
            explicit VariableChanged(EntryPath path, std::string value, std::string old_value)
                : Variable(path, value), old_value(std::move(old_value))
            {}

            [[nodiscard]] auto getOldValue() const -> std::string
            {
                return old_value;
            }

          protected:
            std::string old_value;
        };

        class ValueResponse : sys::ResponseMessage
        {
          public:
            ValueResponse() = default;
            ValueResponse(std::string value) : sys::ResponseMessage(), value(std::move(value))
            {}

            [[nodiscard]] auto getValue() const -> std::string
            {
                return value;
            }

          private:
            std::string value;
        };

        class VariableResponse : public sys::ResponseMessage
        {
          public:
            explicit VariableResponse(EntryPath path,
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
} // namespace settings
