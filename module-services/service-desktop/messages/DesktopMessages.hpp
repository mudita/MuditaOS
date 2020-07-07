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
        ~BackupMessage()
        {}
    };

} // namespace sdesktop
