// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Service/Message.hpp"
#include "MessageType.hpp"
#include "i18/i18.hpp"

#include <module-gui/gui/SwitchData.hpp>

#include <service-appmgr/Types.hpp>
#include <service-appmgr/data/ApplicationManifest.hpp>

namespace app::manager
{
    /// A template for all messages to application manager.
    class Message : public sys::DataMessage
    {
      public:
        Message(MessageType messageType, ApplicationName senderName)
            : sys::DataMessage(messageType), senderName{std::move(senderName)}
        {}

        [[nodiscard]] auto getSenderName() const noexcept -> ApplicationName
        {
            return senderName;
        }

      protected:
        /// name of the application that is sending message to application manager.
        ApplicationName senderName;
    };

    /// Requests a switch to a given application. Optionally to a specified window, too.
    class SwitchRequest : public Message
    {
      public:
        SwitchRequest(const ApplicationName &senderName,
                      ApplicationName applicationName,
                      std::string windowName,
                      std::unique_ptr<gui::SwitchData> data)
            : Message(MessageType::APMSwitch, senderName),
              application{std::move(applicationName)}, window{std::move(windowName)}, data{std::move(data)}
        {}

        [[nodiscard]] auto getName() const noexcept -> const ApplicationName &
        {
            return application;
        }

        [[nodiscard]] auto getWindow() const noexcept -> const std::string &
        {
            return window;
        }

        [[nodiscard]] auto getData() noexcept -> std::unique_ptr<gui::SwitchData> &
        {
            return data;
        }

      private:
        ApplicationName application;
        std::string window;
        std::unique_ptr<gui::SwitchData> data;
    };

    /// Requests a switch to a previous application.
    class SwitchBackRequest : public Message
    {
      public:
        SwitchBackRequest(const ApplicationName &name, std::unique_ptr<gui::SwitchData> data = nullptr)
            : Message(MessageType::APMSwitchPrevApp, name), data{std::move(data)}
        {}

        [[nodiscard]] auto getData() noexcept -> std::unique_ptr<gui::SwitchData> &
        {
            return data;
        }

      private:
        std::unique_ptr<gui::SwitchData> data;
    };

    /// Confirms that the applications lost/gained focus.
    class SwitchConfirmation : public Message
    {
      public:
        SwitchConfirmation(const ApplicationName &name) : Message(MessageType::APMConfirmSwitch, name)
        {}
    };

    /// Confirms that the application closed successfully.
    class CloseConfirmation : public Message
    {
      public:
        CloseConfirmation(const ApplicationName &name) : Message(MessageType::APMConfirmClose, name)
        {}
    };

    /// Confirms that the application registered successfully.
    class ApplicationRegistration : public Message
    {
      public:
        ApplicationRegistration(const ApplicationName &senderName, StartupStatus _status, ApplicationManifest _manifest)
            : Message(MessageType::APMRegister, senderName), status{_status}, manifest{std::move(_manifest)}
        {}

        [[nodiscard]] auto getStatus() const noexcept -> StartupStatus
        {
            return status;
        }

        [[nodiscard]] auto getApplicationManifest() const noexcept -> const ApplicationManifest &
        {
            return manifest;
        }

      private:
        StartupStatus status;
        ApplicationManifest manifest;
    };

    /// Requests the application to close.
    class ApplicationCloseRequest : public Message
    {
      public:
        ApplicationCloseRequest(const ApplicationName &senderName, ApplicationName application)
            : Message(MessageType::APMDelayedClose, senderName), application{std::move(application)}
        {}

        [[nodiscard]] auto getApplication() const noexcept -> const ApplicationName &
        {
            return application;
        }

      private:
        ApplicationName application;
    };

    /// Requests to change the language.
    class LanguageChangeRequest : public Message
    {
      public:
        LanguageChangeRequest(const ApplicationName &senderName, utils::Lang language)
            : Message(MessageType::APMChangeLanguage, senderName), language{language}
        {}

        [[nodiscard]] auto getLanguage() const noexcept -> utils::Lang
        {
            return language;
        }

      private:
        utils::Lang language;
    };

    /// Requests the application manager to close.
    class ShutdownRequest : public Message
    {
      public:
        ShutdownRequest(const ApplicationName &senderName) : Message(MessageType::APMClose, senderName)
        {}
    };

    /// Requests application manager to prevent device blocking.
    class PreventBlockingRequest : public Message
    {
      public:
        PreventBlockingRequest(const ApplicationName &senderName) : Message(MessageType::APMPreventBlocking, senderName)
        {}
    };

    /// Requests the application manager to enter power save mode.
    class PowerSaveModeInitRequest : public Message
    {
      public:
        PowerSaveModeInitRequest(const ApplicationName &senderName)
            : Message(MessageType::APMInitPowerSaveMode, senderName)
        {}
    };

    /// Requests the application manager to check the status of the application.
    class ApplicationStatusRequest : public Message
    {
      public:
        ApplicationStatusRequest(const ApplicationName &senderName, ApplicationName applicationName)
            : Message(MessageType::APMCheckAppRunning, senderName),
              checkAppName(std::move(applicationName)), isRunning{false}
        {}

        ApplicationName checkAppName;
        bool isRunning;
    };

    struct Action
    {
        ApplicationName targetApplication;
        std::string targetWindow;
        std::unique_ptr<gui::SwitchData> data;
    };

    class ActionRequest : public Message
    {
      public:
        ActionRequest(const ApplicationName &senderName, Action &&_action)
            : Message{MessageType::APMAction, senderName}, action{std::move(_action)}
        {}

        auto getAction() noexcept -> Action &
        {
            return action;
        }

      private:
        Action action;
    };
} // namespace app::manager
