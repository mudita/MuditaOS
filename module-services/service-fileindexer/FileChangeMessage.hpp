// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <vfsNotifier.hpp>
namespace service::msg
{
    class FileChangeMessage final : public sys::DataMessage
    {
        using evt_t = vfsn::utility::vfsNotifier::FsEvent;

      public:
        FileChangeMessage(std::string_view new_path, evt_t ev, std::string_view old_path);
        virtual ~FileChangeMessage()
        {}
        FileChangeMessage(FileChangeMessage &) = delete;
        FileChangeMessage &operator=(FileChangeMessage &) = delete;
        auto newPath() const noexcept
        {
            return mNewPath;
        }
        auto oldPath() const noexcept
        {
            return mOldPath;
        }
        auto event() const noexcept
        {
            return mEvent;
        }

      private:
        std::string mNewPath;
        evt_t mEvent;
        std::string mOldPath;
    };
} // namespace service::msg