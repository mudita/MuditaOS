// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/update/UpdateMuditaOS.hpp>
#include <endpoints/developerMode/DeveloperModeEndpoint.hpp>

#include <Service/Message.hpp>
#include <MessageType.hpp>


namespace sdesktop
{
    class UpdateOsMessage : public sys::DataMessage
    {
      public:
        UpdateOsMessage(const std::string updateFilePath, const uint32_t requestUUID)
            : sys::DataMessage(MessageType::UpdateOS)
        {
            updateStats.updateFile = updateFilePath;
            updateStats.uuid       = requestUUID;
        };

        UpdateOsMessage() : sys::DataMessage(MessageType::UpdateOS)
        {}

        UpdateOsMessage(const updateos::UpdateMessageType updateMessageType)
            : sys::DataMessage(MessageType::UpdateOS), messageType(updateMessageType)
        {}

        UpdateOsMessage(const updateos::UpdateMessageType updateMessageType, fs::path updateFileFoundOnBoot)
            : sys::DataMessage(MessageType::UpdateOS), messageType(updateMessageType)
        {
            updateStats.updateFile = updateFileFoundOnBoot;
        }

        ~UpdateOsMessage() override = default;

        updateos::UpdateStats updateStats       = {};
        updateos::UpdateMessageType messageType = updateos::UpdateMessageType::UpdateNow;
    };

    class BackupMessage : public sys::DataMessage
    {
      public:
        BackupMessage() : sys::DataMessage(MessageType::Backup)
        {}
        ~BackupMessage() override = default;
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

    namespace developerMode
    {

        class Event
        {
          protected:
            parserFSM::Context context;

          public:
            void send();
            virtual ~Event() = default;
        };

        class ATResponseEvent : public Event
        {
          public:
            explicit ATResponseEvent(std::vector<std::string> resp);
        };

        class AppFocusChangeEvent : public Event
        {
          public:
            AppFocusChangeEvent() = default;
            explicit AppFocusChangeEvent(std::string appName);
        };

        class ScreenlockCheckEvent : public Event
        {
          public:
            ScreenlockCheckEvent() = default;
            explicit ScreenlockCheckEvent(bool isLocked);
        };

        class DeveloperModeRequest : public sys::DataMessage
        {
          public:
            std::unique_ptr<Event> event;
            DeveloperModeRequest(std::unique_ptr<Event> event);
            DeveloperModeRequest();
            ~DeveloperModeRequest() override = default;
        };
    } // namespace developerMode

} // namespace sdesktop
