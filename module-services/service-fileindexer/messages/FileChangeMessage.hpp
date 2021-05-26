// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include "vfsNotifier.hpp"
namespace service::msg
{
    class FileChangeMessage final : public sys::DataMessage
    {
      public:
        using evt_t = vfsn::utility::vfsNotifier::FsEvent;
        FileChangeMessage(std::string_view new_path, evt_t ev, std::string_view old_path);
        virtual ~FileChangeMessage()           = default;
        FileChangeMessage(FileChangeMessage &) = delete;
        FileChangeMessage &operator=(FileChangeMessage &) = delete;
        [[nodiscard]] auto newPath() const noexcept
        {
            return mNewPath;
        }
        [[nodiscard]] auto oldPath() const noexcept
        {
            return mOldPath;
        }
        [[nodiscard]] auto event() const noexcept
        {
            return mEvent;
        }

      private:
        const std::string mNewPath;
        const evt_t mEvent;
        const std::string mOldPath;
    };
} // namespace service::msg
