#pragma once

#include "Service/Message.hpp"
#include "MessageType.hpp"

namespace sdesktop
{
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
