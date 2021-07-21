// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/SwitchData.hpp>
#include <service-cellular/PacketDataTypes.hpp>

namespace gui
{

    class ApnListData : public SwitchData
    {
      public:
        explicit ApnListData(std::vector<std::shared_ptr<packet_data::APN::Config>> apns) : apns(std::move(apns))
        {}
        [[nodiscard]] auto getAPNs() const noexcept -> const std::vector<std::shared_ptr<packet_data::APN::Config>> &
        {
            return apns;
        }

      private:
        std::vector<std::shared_ptr<packet_data::APN::Config>> apns;
    };
} // namespace gui
