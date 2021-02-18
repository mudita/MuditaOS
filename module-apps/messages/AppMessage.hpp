// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common.hpp"
#include "SwitchData.hpp"
#include "gui/input/InputEvent.hpp"
#include "BaseAppMessage.hpp"
#include <memory>
#include <string>

namespace app
{

    class AppActionRequest : public AppMessage
    {
      public:
        AppActionRequest(manager::actions::ActionId _actionId, manager::actions::ActionParamsPtr _data)
            : AppMessage(MessageType::AppAction), actionId{_actionId}, data{std::move(_data)}
        {}

        manager::actions::ActionId getAction() const noexcept
        {
            return actionId;
        }

        manager::actions::ActionParamsPtr &getData() noexcept
        {
            return data;
        }

      private:
        manager::actions::ActionId actionId;
        manager::actions::ActionParamsPtr data;
    };

    // this message is used to notify application about switching event. Application will gain or lose focus upon
    // receiving this message. Application gains focus when it was in init or active background state. Application loose
    // focus when it was in active foreground state. if no window is specified it is assumed that MainWindow is the
    // target
    class AppSwitchMessage : public AppMessage
    {
      protected:
        // name of the application to which switch is performed.
        std::string targetApplication;
        // name of the window to which switch should be performed.
        std::string targetWindow;
        // optional data for the target window.
        std::unique_ptr<gui::SwitchData> data;
        // name of the application to which switch should be performed after finishing tasks in target application and
        // window.
        std::string returnApplication;
        // name of the window to which switch should be performed after finishing tasks in target application and
        // window.
        std::string returnWindow;

      public:
        AppSwitchMessage(const std::string &targetApplication,
                         const std::string &targetWindow,
                         std::unique_ptr<gui::SwitchData> data,
                         const std::string &returnApplication = "",
                         const std::string &returnWindow      = "")
            : AppMessage(MessageType::AppSwitch), targetApplication{targetApplication}, targetWindow{targetWindow},
              data{std::move(data)}, returnApplication{returnApplication}, returnWindow{returnWindow} {};
        virtual ~AppSwitchMessage(){};

        std::string getTargetWindowName()
        {
            return targetWindow;
        };
        std::string getReturnWindowName()
        {
            return returnWindow;
        };
        std::unique_ptr<gui::SwitchData> &getData()
        {
            return data;
        };
        std::string getTargetApplicationName()
        {
            return targetApplication;
        };
        std::string getReturnApplicationName()
        {
            return returnApplication;
        };
    };

    class AppRefreshMessage : public AppMessage
    {
      protected:
        gui::RefreshModes mode;
        std::string window_name;

      public:
        AppRefreshMessage(gui::RefreshModes mode, std::string window_name)
            : mode{mode}, window_name(std::move(window_name)){};

        [[nodiscard]] const gui::RefreshModes &getMode() const
        {
            return mode;
        }

        [[nodiscard]] const std::string &getWindowName() const
        {
            return window_name;
        }
    };

    class AppSwitchWindowMessage : public AppMessage
    {
      protected:
        std::string window;
        std::string senderWindow;
        gui::ShowMode command;
        std::unique_ptr<gui::SwitchData> data;

      public:
        AppSwitchWindowMessage() = delete;

        AppSwitchWindowMessage(const std::string &window,
                               const std::string senderWindow,
                               std::unique_ptr<gui::SwitchData> data,
                               const gui::ShowMode command = gui::ShowMode::GUI_SHOW_INIT)
            : AppMessage(MessageType::AppSwitchWindow), window{window},
              senderWindow{senderWindow}, command{command}, data{std::move(data)} {};
        virtual ~AppSwitchWindowMessage() = default;

        const std::string &getWindowName() const
        {
            return window;
        };
        const std::string &getSenderWindowName() const
        {
            return senderWindow;
        };
        const gui::ShowMode &getCommand() const
        {
            return command;
        };
        std::unique_ptr<gui::SwitchData> &getData()
        {
            return data;
        };
    };

    class AppInputEventMessage : public AppMessage
    {
      protected:
        gui::InputEvent event;

      public:
        AppInputEventMessage(gui::InputEvent evt) : AppMessage(MessageType::AppInputEvent), event{evt} {};
        virtual ~AppInputEventMessage(){};

        const gui::InputEvent &getEvent()
        {
            return event;
        };

        operator std::string() const override
        {
            std::stringstream ss;
            ss << "{ ";
            ss << "state:   " << c_str(event.state) << ", ";
            ss << "RawKey:  " << c_str(event.key.key_code) << "}";
            ss << "t0: " << event.key.time_press << ", t1: " << event.key.time_release;
            ss << " }";
            return ss.str().c_str();
        }
    };

    class AppRebuildMessage : public AppMessage
    {
      public:
        AppRebuildMessage() : AppMessage(MessageType::AppRebuild){};
        virtual ~AppRebuildMessage(){};
    };

    class AppLostFocusMessage : public AppMessage
    {
      public:
        AppLostFocusMessage() : AppMessage{MessageType::AppFocusLost}
        {}
    };
};     // namespace app
