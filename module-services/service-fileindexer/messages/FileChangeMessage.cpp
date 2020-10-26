// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FileChangeMessage.hpp"

namespace service::msg
{
    FileChangeMessage::FileChangeMessage(std::string_view new_path, evt_t ev, std::string_view old_path)
        : DataMessage(MessageType::FileContentModified), mNewPath(new_path), mEvent(ev), mOldPath(old_path)
    {}
} // namespace service::msg