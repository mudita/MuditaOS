// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/SwitchData.hpp>
#include <service-cellular/service-cellular/PacketDataTypes.hpp>

class ApnItemData : public gui::SwitchData
{
  public:
    ApnItemData(std::shared_ptr<packet_data::APN::Config> Apn) : apn(std::move(Apn)){};
    ApnItemData() : apn(nullptr){};

    auto getApn() -> std::shared_ptr<packet_data::APN::Config>
    {
        return apn;
    }

  private:
    std::shared_ptr<packet_data::APN::Config> apn;
};
