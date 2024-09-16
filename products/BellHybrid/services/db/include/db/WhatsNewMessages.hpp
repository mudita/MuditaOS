// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>
#include <vector>

namespace service::db::whatsnew
{
    struct Record
    {
        std::string title;
        std::string description;
        std::string iconName;
    };

    struct VersionNumber
    {
        std::uint16_t major;
        std::uint16_t minor;
        std::uint16_t patch;
    };

    namespace messages
    {
        struct GetByVersion : public sys::DataMessage
        {
            explicit GetByVersion(VersionNumber version) : version{version}
            {}
            VersionNumber version;
        };

        struct Response : public sys::ResponseMessage
        {
            Response() = default;
            explicit Response(const std::vector<Record> &records) : records{records}
            {}

            std::vector<Record> records{};
        };
    } // namespace messages
} // namespace service::db::whatsnew
