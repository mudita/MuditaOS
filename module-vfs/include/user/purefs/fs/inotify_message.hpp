// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <Service/Message.hpp>
#include <purefs/fs/inotify_flags.hpp>

namespace purefs::fs::message
{
    //! Inotify message received by the notifier class
    struct inotify final : public ::sys::DataMessage
    {
        inotify(inotify_flags _flags, std::string_view _name, std::string_view _name_prev)
            : flags(_flags), name(_name), name_prev(_name_prev)
        {}
        virtual ~inotify() = default;
        const inotify_flags flags;
        const std::string name;
        const std::string name_prev;
    };
} // namespace message
