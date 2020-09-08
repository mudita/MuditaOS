#pragma once

#include "Service/Message.hpp"
#include "MessageType.hpp"
#include <vfs.hpp>

namespace sdesktop
{
    enum UpdateMessageType
    {
        UpdateFoundOnBoot,
        UpdateCheckForUpdateOnce,
        UpdateNow,
        UpdateError,
        UpdateInform
    };

    class UpdateOsMessage : public sys::DataMessage
    {
    public:
        UpdateOsMessage(const std::string updateFilePath, const uint32_t requestUUID)
                : sys::DataMessage(MessageType::UpdateOS), updateFile(updateFilePath), uuid(requestUUID)
                {};

        UpdateOsMessage() : sys::DataMessage(MessageType::UpdateOS)
        {}

        UpdateOsMessage(const sdesktop::UpdateMessageType updateMessageType)
                : sys::DataMessage(MessageType::UpdateOS), messageType(updateMessageType)
        {}

        UpdateOsMessage(const sdesktop::UpdateMessageType updateMessageType, fs::path updateFileFoundOnBoot)
                : sys::DataMessage(MessageType::UpdateOS), updateFile(updateFileFoundOnBoot), messageType(updateMessageType)
        {}

        ~UpdateOsMessage() override = default;

        fs::path updateFile           = "";
        uint32_t uuid                 = 0;
        UpdateMessageType messageType = UpdateNow;
        std::string messageText       = "";
        uint8_t status                = 0;
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
} // namespace sdesktop
