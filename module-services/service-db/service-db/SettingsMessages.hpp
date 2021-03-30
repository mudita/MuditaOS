// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>
#include "EntryPath.hpp"

#include <list>
#include <memory>
#include <set>
#include <utility>
#include <variant>

namespace settings
{

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
                : path(std::move(path)), value(std::move(value))
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
            explicit GetVariable(EntryPath path) : Variable(std::move(path))
            {}
        };

        class SetVariable : public Variable
        {
          public:
            SetVariable() = default;
            SetVariable(EntryPath path, std::string value) : Variable(std::move(path), value)
            {}
        };

        class RegisterOnVariableChange : public Variable
        {
          public:
            RegisterOnVariableChange() = default;
            explicit RegisterOnVariableChange(EntryPath path) : Variable(std::move(path))
            {}
        };

        class UnregisterOnVariableChange : public Variable
        {
          public:
            UnregisterOnVariableChange() = default;
            explicit UnregisterOnVariableChange(EntryPath path) : Variable(std::move(path))
            {}
        };

        class VariableChanged : public Variable
        {
          public:
            VariableChanged() = default;
            explicit VariableChanged(EntryPath path, std::string value, std::string old_value)
                : Variable(std::move(path), value), old_value(std::move(old_value))
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
