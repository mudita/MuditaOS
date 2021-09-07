// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/Application.hpp>
#include <service-cellular/CellularServiceAPI.hpp>
#include <service-cellular/PacketDataTypes.hpp>

class ApnSettingsModel
{
  public:
    explicit ApnSettingsModel(app::Application *application);

    void requestAPNList();
    void saveAPN(const std::shared_ptr<packet_data::APN::Config> &apn);
    void removeAPN(const std::shared_ptr<packet_data::APN::Config> &apn);
    void setAsDefaultAPN(const std::shared_ptr<packet_data::APN::Config> &apn);

  private:
    app::Application *application = nullptr;
};
