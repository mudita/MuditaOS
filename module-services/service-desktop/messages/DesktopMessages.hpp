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

    struct UpdateStats
    {
        fs::path updateFile = "";
        fs::path fileExtracted = "";
        fs::path updateTempDirectory = PATH_SYS "/" PATH_TMP;
        uint32_t totalBytes = 0;
        uint32_t currentExtractedBytes = 0;
        uint32_t uuid = 0;
        std::string messageText = "";
        uint8_t status;
        json11::Json versioInformation;
    };

    class UpdateOsMessage : public sys::DataMessage
    {
    public:
        UpdateOsMessage(const std::string updateFilePath, const uint32_t requestUUID)
                : sys::DataMessage(MessageType::UpdateOS)
                {
                    updateStats.updateFile = updateFilePath;
                    updateStats.uuid = requestUUID;
                };

        UpdateOsMessage() : sys::DataMessage(MessageType::UpdateOS) {}

        UpdateOsMessage(const sdesktop::UpdateMessageType updateMessageType)
                : sys::DataMessage(MessageType::UpdateOS), messageType(updateMessageType) {}

        UpdateOsMessage(const sdesktop::UpdateMessageType updateMessageType, fs::path updateFileFoundOnBoot)
                : sys::DataMessage(MessageType::UpdateOS), messageType(updateMessageType)
        {
            updateStats.updateFile = updateFileFoundOnBoot;
        }

        ~UpdateOsMessage() override = default;

        UpdateStats updateStats = {};
        UpdateMessageType messageType = UpdateNow;
        int rebootDelay = 0;
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
