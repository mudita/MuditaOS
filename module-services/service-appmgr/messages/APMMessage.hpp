// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Service/Message.hpp"
#include "MessageType.hpp"

#include "SwitchData.hpp"

#include "i18/i18.hpp"

namespace app::manager
{
    /// A template for all messages to application manager.
    class APMMessage : public sys::DataMessage
    {
      protected:
        /// name of the application that is sending message to application manager.
        std::string senderName;

      public:
        APMMessage(MessageType messageType, std::string senderName)
            : sys::DataMessage(messageType), senderName{std::move(senderName)}
        {}

        [[nodiscard]] auto getSenderName() const noexcept -> std::string
        {
            return senderName;
        }
    };

    /// Requests a switch to a given application. Optionally to a specified window, too.
    class APMSwitch : public APMMessage
    {
        std::string application;
        std::string window;
        std::unique_ptr<gui::SwitchData> data;

      public:
        APMSwitch(const std::string &senderName,
                  std::string applicationName,
                  std::string windowName,
                  std::unique_ptr<gui::SwitchData> data)
            : APMMessage(MessageType::APMSwitch, senderName),
              application{std::move(applicationName)}, window{std::move(windowName)}, data{std::move(data)}
        {}

        [[nodiscard]] auto getName() const noexcept -> const std::string &
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
    };

    /// Requests a switch to a previous application.
    class APMSwitchPrevApp : public APMMessage
    {
        std::unique_ptr<gui::SwitchData> data;

      public:
        APMSwitchPrevApp(const std::string &name, std::unique_ptr<gui::SwitchData> data = nullptr)
            : APMMessage(MessageType::APMSwitchPrevApp, name), data{std::move(data)}
        {}

        [[nodiscard]] auto getData() noexcept -> std::unique_ptr<gui::SwitchData> &
        {
            return data;
        }
    };

    /// Confirms that the applications lost/gained focus.
    class APMConfirmSwitch : public APMMessage
    {
      public:
        APMConfirmSwitch(const std::string &name) : APMMessage(MessageType::APMConfirmSwitch, name)
        {}
    };

    /// Confirms that the application closed successfully.
    class APMConfirmClose : public APMMessage
    {
      public:
        APMConfirmClose(const std::string &name) : APMMessage(MessageType::APMConfirmClose, name)
        {}
    };

    /// Confirms that the application registered successfully.
    class APMRegister : public APMMessage
    {
        bool status;
        bool startBackground;

      public:
        APMRegister(const std::string &senderName, bool status, bool startBackground)
            : APMMessage(MessageType::APMRegister, senderName), status{status}, startBackground{startBackground}
        {}

        [[nodiscard]] auto getStatus() const noexcept -> bool
        {
            return status;
        }

        [[nodiscard]] auto getStartBackground() const noexcept -> bool
        {
            return startBackground;
        }
    };

    /// Requests the application to close.
    class APMDelayedClose : public APMMessage
    {
        std::string application;

      public:
        APMDelayedClose(const std::string &senderName, std::string application)
            : APMMessage(MessageType::APMDelayedClose, senderName), application{std::move(application)}
        {}

        [[nodiscard]] auto getApplication() const noexcept -> const std::string &
        {
            return application;
        }
    };

    /// Requests to change the language.
    class APMChangeLanguage : public APMMessage
    {
        utils::Lang language;

      public:
        APMChangeLanguage(const std::string &senderName, utils::Lang language)
            : APMMessage(MessageType::APMChangeLanguage, senderName), language{language}
        {}

        [[nodiscard]] auto getLanguage() const noexcept -> utils::Lang
        {
            return language;
        }
    };

    /// Requests the application manager to close.
    class APMClose : public APMMessage
    {
      public:
        APMClose(const std::string &senderName) : APMMessage(MessageType::APMClose, senderName)
        {}
    };

    /// Requests application manager to prevent device blocking.
    class APMPreventBlocking : public APMMessage
    {
      public:
        APMPreventBlocking(const std::string &senderName) : APMMessage(MessageType::APMPreventBlocking, senderName)
        {}
    };

    /// Requests the application manager to enter power save mode.
    class APMInitPowerSaveMode : public APMMessage
    {
      public:
        APMInitPowerSaveMode(const std::string &senderName) : APMMessage(MessageType::APMInitPowerSaveMode, senderName)
        {}
    };

    /// Requests the application manager to check the status of the application.
    class APMCheckApp : public APMMessage
    {
      public:
        APMCheckApp(const std::string &senderName, std::string applicationName)
            : APMMessage(MessageType::APMCheckAppRunning, senderName),
              checkAppName(std::move(applicationName)), isRunning{false}
        {}

        std::string checkAppName;
        bool isRunning;
    };

    struct Action
    {
        std::string targetApplication;
        std::string targetWindow;
        std::unique_ptr<gui::SwitchData> data;
    };

    class APMAction : public APMMessage
    {
      public:
        APMAction(const std::string &senderName, Action &&_action)
            : APMMessage{MessageType::APMAction, senderName}, action{std::move(_action)}
        {}

        auto getAction() noexcept -> Action &
        {
            return action;
        }

      private:
        Action action;
    };
} // namespace app::manager
