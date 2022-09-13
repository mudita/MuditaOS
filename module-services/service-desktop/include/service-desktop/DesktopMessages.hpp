// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/Actions.hpp>
#include <service-appmgr/messages/ActionRequest.hpp>
#include <Service/Message.hpp>
#include <MessageType.hpp>
#include <service-desktop/DeveloperModeMessage.hpp>
#include <service-desktop/DesktopEvent.hpp>

namespace sdesktop
{
    class BackupMessage : public sys::DataMessage
    {
      public:
        BackupMessage() : sys::DataMessage(MessageType::Backup)
        {}
        ~BackupMessage() override = default;
    };

    class SyncMessage : public sys::DataMessage
    {
      public:
        SyncMessage() : sys::DataMessage(MessageType::Sync)
        {}
        ~SyncMessage() override = default;
    };

    class RestoreMessage : public sys::DataMessage
    {
      public:
        RestoreMessage() : sys::DataMessage(MessageType::Restore)
        {}
        ~RestoreMessage() override = default;
    };

    class FactoryMessage : public sys::DataMessage
    {
      public:
        FactoryMessage() : sys::DataMessage(MessageType::Factory)
        {}
        ~FactoryMessage() override = default;
    };

    namespace usb
    {
        enum class USBConfigurationType
        {
            firstConfiguration,
            reconfiguration
        };

        class USBConnected : public sys::DataMessage
        {
          public:
            USBConnected() : sys::DataMessage(MessageType::USBConnected)
            {}
            ~USBConnected() override = default;
        };

        class USBConfigured : public sys::DataMessage
        {
          public:
            explicit USBConfigured(
                USBConfigurationType configurationType = sdesktop::usb::USBConfigurationType::firstConfiguration);
            ~USBConfigured() override = default;
            USBConfigurationType getConfigurationType() const noexcept;

          private:
            USBConfigurationType configurationType;
        };

        class USBDisconnected : public sys::DataMessage
        {
          public:
            USBDisconnected() : sys::DataMessage(MessageType::USBDisconnected)
            {}
            ~USBDisconnected() override = default;
        }; // namespace usb

    } // namespace usb

    namespace developerMode
    {
        class AppFocusChangeEvent : public Event
        {
          public:
            AppFocusChangeEvent() = default;
            explicit AppFocusChangeEvent(std::string appName);
        };

        class CellularHotStartEvent : public Event
        {
          public:
            CellularHotStartEvent() = default;
        };
        class CellularStateInfoRequestEvent : public Event
        {
          public:
            CellularStateInfoRequestEvent() = default;
            explicit CellularStateInfoRequestEvent(std::string stateStr);
        };
        class CellularSleepModeInfoRequestEvent : public Event
        {
          public:
            CellularSleepModeInfoRequestEvent() = default;
            explicit CellularSleepModeInfoRequestEvent(bool isInSleepMode);
        };
        class ScreenlockCheckEvent : public Event
        {
          public:
            ScreenlockCheckEvent() = default;
            explicit ScreenlockCheckEvent(bool isLocked);
        };
    } // namespace developerMode
} // namespace sdesktop
