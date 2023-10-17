// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common.hpp"
#include "popups/Disposition.hpp"
#include "SwitchData.hpp"
#include "gui/input/InputEvent.hpp"
#include "BaseAppMessage.hpp"
#include "AppSwitchReason.hpp"
#include "AppStartupReason.hpp"
#include <memory>
#include <string>
#include <service-appmgr/Actions.hpp>
#include <utility>

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

    class ActionHandledResponse : public sys::ResponseMessage
    {
      public:
        explicit ActionHandledResponse(sys::ReturnCodes retCode = sys::ReturnCodes::Success)
            : sys::ResponseMessage{retCode, MessageType::AppAction}
        {}
    };

    // this message is used to notify application about switching event. Application will gain or lose focus upon
    // receiving this message. Application gains focus when it was in init or active background state. Application loose
    // focus when it was in active foreground state. if no window is specified it is assumed that MainWindow is the
    // target
    class AppSwitchMessage : public AppMessage
    {
      private:
        // name of the application to which switch is performed.
        std::string targetApplication;
        // name of the window to which switch should be performed.
        std::string targetWindow;
        // optional data for the target window.
        std::unique_ptr<gui::SwitchData> data;
        StartupReason startupReason = StartupReason::Launch;

      public:
        AppSwitchMessage(const std::string &targetApplication,
                         const std::string &targetWindow,
                         std::unique_ptr<gui::SwitchData> data,
                         StartupReason startupReason)
            : AppMessage(MessageType::AppSwitch), targetApplication{targetApplication},
              targetWindow{targetWindow}, data{std::move(data)}, startupReason{startupReason} {};
        virtual ~AppSwitchMessage(){};

        [[nodiscard]] std::string getTargetWindowName() const noexcept
        {
            return targetWindow;
        };

        [[nodiscard]] std::unique_ptr<gui::SwitchData> &getData()
        {
            return data;
        };

        [[nodiscard]] std::string getTargetApplicationName() const noexcept
        {
            return targetApplication;
        };

        [[nodiscard]] StartupReason getApplicationStartupReason() const noexcept
        {
            return startupReason;
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
        SwitchReason reason;
        std::unique_ptr<gui::SwitchData> data;

      public:
        AppSwitchWindowMessage() = delete;

        AppSwitchWindowMessage(const std::string &window,
                               const std::string senderWindow,
                               std::unique_ptr<gui::SwitchData> data,
                               gui::ShowMode command = gui::ShowMode::GUI_SHOW_INIT,
                               SwitchReason reason   = SwitchReason::SwitchRequest)
            : AppMessage(MessageType::AppSwitchWindow), window{window},
              senderWindow{senderWindow}, command{command}, reason{reason}, data{std::move(data)} {};

        virtual ~AppSwitchWindowMessage() = default;

        [[nodiscard]] virtual bool toPopupRequest() const
        {
            return false;
        }

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
        [[nodiscard]] SwitchReason getReason() const noexcept
        {
            return reason;
        }
        std::unique_ptr<gui::SwitchData> &getData()
        {
            return data;
        };

        virtual std::pair<const std::string &, gui::popup::Disposition> getSwitchData()
        {
            return {window, gui::popup::WindowDisposition};
        }
    };

    class AppUpdateWindowMessage : public AppMessage
    {
      private:
        const std::string window;
        const std::unique_ptr<gui::SwitchData> data;
        const gui::ShowMode command;
        const gui::RefreshModes refreshMode;

      public:
        AppUpdateWindowMessage(const std::string &window,
                               std::unique_ptr<gui::SwitchData> data,
                               gui::ShowMode command         = gui::ShowMode::GUI_SHOW_INIT,
                               gui::RefreshModes refreshMode = gui::RefreshModes::GUI_REFRESH_FAST)
            : AppMessage(), window{window}, data{std::move(data)}, command{command}, refreshMode{refreshMode}
        {}

        [[nodiscard]] auto getWindowName() const
        {
            return window;
        }

        [[nodiscard]] const auto &getData() const noexcept
        {
            return data;
        }

        [[nodiscard]] auto getCommand() const noexcept
        {
            return command;
        }

        [[nodiscard]] auto getRefreshMode() const noexcept
        {
            return refreshMode;
        }
    };

    class AppInputEventMessage : public AppMessage
    {
      protected:
        gui::InputEvent event;

      public:
        explicit AppInputEventMessage(gui::InputEvent evt) : AppMessage(MessageType::AppInputEvent), event{evt}
        {}

        const gui::InputEvent &getEvent()
        {
            return event;
        };

        operator std::string() const override
        {
            const auto &rawKey = event.getRawKey();
            std::stringstream ss;
            ss << "{ ";
            ss << "state:   " << c_str(event.getState()) << ", ";
            ss << "RawKey:  " << c_str(rawKey.keyCode) << "}";
            ss << "t0: " << rawKey.timePress << ", t1: " << rawKey.timeRelease;
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

    class AppSwitchBackMessage : public AppMessage
    {
      public:
        AppSwitchBackMessage() : AppMessage(MessageType::AppSwitchBack)
        {}
    };
} // namespace app
